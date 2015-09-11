#pragma once

#include "MessageBuffer.h"
#include <atomic>
#include <vector>
#include <mutex>
#include <queue>
#include <memory>
#include <functional>
#include <type_traits>
#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

#define READ_BLOCK_SIZE 4096
#ifdef BOOST_ASIO_HAS_IOCP
#define TC_SOCKET_USE_IOCP
#endif

template<class T>
class Socket : public std::enable_shared_from_this<T>
{
public:
	explicit Socket(tcp::socket&& socket) : _socket(std::move(socket)), 
		_remoteAddress(_socket.remote_endpoint().address()),
		_remotePort(_socket.remote_endpoint().port()), 
		_readBuffer(), _closed(false), _closing(false), _isWritingAsync(false)
	{
		_readBuffer.Resize(READ_BLOCK_SIZE);
	}

	explicit Socket(tcp::socket&& socket, bool bClient) : _socket(std::move(socket)),
		//_remoteAddress(_socket.remote_endpoint().address()),
		//_remotePort(_socket.remote_endpoint().port()),
		_readBuffer(), _closed(false), _closing(false), _isWritingAsync(false)
	{
		_readBuffer.Resize(READ_BLOCK_SIZE);
	}

	virtual ~Socket()
	{
		_closed = true;
		boost::system::error_code error;
		_socket.close(error);
	}

	virtual void Start() = 0;

	virtual bool Update()
	{
		if (!IsOpen())
			return false;

#ifndef TC_SOCKET_USE_IOCP
		std::unique_lock<std::mutex> guard(_writeLock);
		if (!guard)
			return true;

		if (_isWritingAsync || (!_writeBuffer.GetActiveSize() && _writeQueue.empty()))
			return true;

		for (; WriteHandler(guard);)
			;
#endif

		return true;
	}

	boost::asio::ip::address GetRemoteIpAddress() const
	{
		return _remoteAddress;
	}

	uint16 GetRemotePort() const
	{
		return _remotePort;
	}

	void Connect(std::string strIP, std::string strPort)
	{
		tcp::resolver resolver(io_service());
		tcp::resolver::query query(strIP, strPort);
		tcp::resolver::iterator iterator = resolver.resolve(query);
        

		boost::asio::async_connect(_socket, iterator,
			bind(&Socket<T>::ConnectHandlerInternal, this->shared_from_this(),
				boost::asio::placeholders::error));
	}

	void ConnectHandlerInternal(boost::system::error_code error)
	{
        std::cout << "client link" << std::endl;
		ConnectHandler();
		AsyncRead();
	}

	void AsyncRead()
	{
		if (!IsOpen())
			return;

		_readBuffer.Normalize();
		_readBuffer.EnsureFreeSpace();
		_socket.async_read_some(boost::asio::buffer(_readBuffer.GetWritePointer(), _readBuffer.GetRemainingSpace()),
			std::bind(&Socket<T>::ReadHandlerInternal, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void QueuePacket(MessageBuffer&& buffer, std::unique_lock<std::mutex>& guard)
	{
		_writeQueue.push(std::move(buffer));

//#ifdef TC_SOCKET_USE_IOCP
		AsyncProcessQueue(guard);
//#else
//		(void)guard;
//#endif
	}

	bool IsOpen() const { return !_closed && !_closing; }

	void CloseSocket()
	{
		if (_closed.exchange(true))
			return;

		boost::system::error_code shutdownError;
		_socket.shutdown(boost::asio::socket_base::shutdown_send, shutdownError);
		//if (shutdownError)
			//TC_LOG_DEBUG("network", "Socket::CloseSocket: %s errored when shutting down socket: %i (%s)", GetRemoteIpAddress().to_string().c_str(),
				//shutdownError.value(), shutdownError.message().c_str());

		OnClose();
	}

	/// Marks the socket for closing after write buffer becomes empty
	void DelayedCloseSocket() { _closing = true; }

	MessageBuffer& GetReadBuffer() { return _readBuffer; }

protected:
	virtual void OnClose() { }

	virtual void ReadHandler() = 0;
	virtual void ConnectHandler() {};

	bool AsyncProcessQueue(std::unique_lock<std::mutex>&)
	{
		if (_isWritingAsync)
			return false;

		_isWritingAsync = true;

#ifdef TC_SOCKET_USE_IOCP
		MessageBuffer& buffer = _writeQueue.front();
		_socket.async_write_some(boost::asio::buffer(buffer.GetReadPointer(), buffer.GetActiveSize()), std::bind(&Socket<T>::WriteHandler,
			this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#else
		//_writeBuffer = _writeQueue.front();
        _socket.async_write_some(boost::asio::null_buffers(), std::bind(&Socket<T>::WriteHandlerWrapper,
			this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#endif

		return false;
	}

	void SetNoDelay(bool enable)
	{
		boost::system::error_code err;
		_socket.set_option(boost::asio::ip::tcp::no_delay(enable), err);
		if (err)
			TC_LOG_DEBUG("network", "Socket::SetNoDelay: failed to set_option(boost::asio::ip::tcp::no_delay) for %s - %d (%s)",
				GetRemoteIpAddress().to_string().c_str(), err.value(), err.message().c_str());
	}

	std::mutex _writeLock;
	std::queue<MessageBuffer> _writeQueue;
#ifndef TC_SOCKET_USE_IOCP
	MessageBuffer _writeBuffer;
#endif

	boost::asio::io_service& io_service() { return _socket.get_io_service(); }

private:
	void ReadHandlerInternal(boost::system::error_code error, size_t transferredBytes)
	{
		if (error)
		{
			CloseSocket();
			return;
		}

		_readBuffer.WriteCompleted(transferredBytes);
		ReadHandler();
	}

#ifdef TC_SOCKET_USE_IOCP

	void WriteHandler(boost::system::error_code error, std::size_t transferedBytes)
	{
		if (!error)
		{
			std::unique_lock<std::mutex> deleteGuard(_writeLock);

			_isWritingAsync = false;
			_writeQueue.front().ReadCompleted(transferedBytes);
			if (!_writeQueue.front().GetActiveSize())
				_writeQueue.pop();

			if (!_writeQueue.empty())
				AsyncProcessQueue(deleteGuard);
			else if (_closing)
				CloseSocket();
		}
		else
			CloseSocket();
	}

#else

	void WriteHandlerWrapper(boost::system::error_code /*error*/, std::size_t /*transferedBytes*/)
	{
		std::unique_lock<std::mutex> guard(_writeLock);
		_isWritingAsync = false;
		WriteHandler(guard);
	}

	bool WriteHandler(std::unique_lock<std::mutex>& guard)
	{
		if (!IsOpen())
			return false;

		std::size_t bytesToSend = _writeBuffer.GetActiveSize();

		if (bytesToSend == 0)
			return HandleQueue(guard);

		boost::system::error_code error;
		std::size_t bytesWritten = _socket.write_some(boost::asio::buffer(_writeBuffer.GetReadPointer(), bytesToSend), error);

		if (error)
		{
			if (error == boost::asio::error::would_block || error == boost::asio::error::try_again)
				return AsyncProcessQueue(guard);

			return false;
		}
		else if (bytesWritten == 0)
			return false;
		else if (bytesWritten < bytesToSend)
		{
			_writeBuffer.ReadCompleted(bytesWritten);
			_writeBuffer.Normalize();
			return AsyncProcessQueue(guard);
		}

		// now bytesWritten == bytesToSend
		_writeBuffer.Reset();

		return HandleQueue(guard);
	}

	bool HandleQueue(std::unique_lock<std::mutex>& guard)
	{
		if (_writeQueue.empty())
			return false;

		MessageBuffer& queuedMessage = _writeQueue.front();

		std::size_t bytesToSend = queuedMessage.GetActiveSize();

		boost::system::error_code error;
		std::size_t bytesSent = _socket.write_some(boost::asio::buffer(queuedMessage.GetReadPointer(), bytesToSend), error);

		if (error)
		{
			if (error == boost::asio::error::would_block || error == boost::asio::error::try_again)
				return AsyncProcessQueue(guard);

			_writeQueue.pop();
			return false;
		}
		else if (bytesSent == 0)
		{
			_writeQueue.pop();
			return false;
		}
		else if (bytesSent < bytesToSend) // now n > 0
		{
			queuedMessage.ReadCompleted(bytesSent);
			return AsyncProcessQueue(guard);
		}

		_writeQueue.pop();
		return !_writeQueue.empty();
	}

#endif

	tcp::socket _socket;

	boost::asio::ip::address _remoteAddress;
	uint16 _remotePort;

	MessageBuffer _readBuffer;

	std::atomic<bool> _closed;
	std::atomic<bool> _closing;

	bool _isWritingAsync;
};