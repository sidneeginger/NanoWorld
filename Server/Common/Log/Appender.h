#pragma once

#include <unordered_map>
#include <stdexcept>
#include <string>
#include <time.h>
#include <type_traits>
#include <vector>
#include <utility>
#include "Define.h"

// Values assigned have their equivalent in enum ACE_Log_Priority
enum LogLevel
{
	LOG_LEVEL_DISABLED = 0,
	LOG_LEVEL_TRACE = 1,
	LOG_LEVEL_DEBUG = 2,
	LOG_LEVEL_INFO = 3,
	LOG_LEVEL_WARN = 4,
	LOG_LEVEL_ERROR = 5,
	LOG_LEVEL_FATAL = 6
};

const uint8 MaxLogLevels = 6;

enum AppenderType
{
	APPENDER_NONE,
	APPENDER_CONSOLE,
	APPENDER_FILE,
	APPENDER_DB
};

enum AppenderFlags
{
	APPENDER_FLAGS_NONE = 0x00,
	APPENDER_FLAGS_PREFIX_TIMESTAMP = 0x01,
	APPENDER_FLAGS_PREFIX_LOGLEVEL = 0x02,
	APPENDER_FLAGS_PREFIX_LOGFILTERTYPE = 0x04,
	APPENDER_FLAGS_USE_TIMESTAMP = 0x08, // only used by FileAppender
	APPENDER_FLAGS_MAKE_FILE_BACKUP = 0x10  // only used by FileAppender
};

struct LogMessage
{
	LogMessage(LogLevel _level, std::string const& _type, std::string&& _text)
		: level(_level), type(_type), text(std::forward<std::string>(_text)), mtime(time(NULL))
	{ }

	LogMessage(LogMessage const& /*other*/) = delete;
	LogMessage& operator=(LogMessage const& /*other*/) = delete;

	static std::string getTimeStr(time_t time);
	std::string getTimeStr();

	LogLevel const level;
	std::string const type;
	std::string const text;
	std::string prefix;
	std::string param1;
	time_t mtime;

	///@ Returns size of the log message content in bytes
	uint32 Size() const
	{
		return static_cast<uint32>(prefix.size() + text.size());
	}
};

class Appender
{
public:
	Appender(uint8 _id, std::string const& name, LogLevel level = LOG_LEVEL_DISABLED, AppenderFlags flags = APPENDER_FLAGS_NONE);
	virtual ~Appender();

	uint8 getId() const;
	std::string const& getName() const;
	virtual AppenderType getType() const = 0;
	LogLevel getLogLevel() const;
	AppenderFlags getFlags() const;

	void setLogLevel(LogLevel);
	void write(LogMessage* message);
	static const char* getLogLevelString(LogLevel level);
	virtual void setRealmId(uint32 /*realmId*/) { }

private:
	virtual void _write(LogMessage const* /*message*/) = 0;

	uint8 id;
	std::string name;
	LogLevel level;
	AppenderFlags flags;
};

typedef std::unordered_map<uint8, Appender*> AppenderMap;

typedef std::vector<char const*> ExtraAppenderArgs;
typedef Appender*(*AppenderCreatorFn)(uint8 id, std::string const& name, LogLevel level, AppenderFlags flags, ExtraAppenderArgs extraArgs);
typedef std::unordered_map<uint8, AppenderCreatorFn> AppenderCreatorMap;

template<class AppenderImpl>
Appender* CreateAppender(uint8 id, std::string const& name, LogLevel level, AppenderFlags flags, ExtraAppenderArgs extraArgs)
{
	return new AppenderImpl(id, name, level, flags, std::forward<ExtraAppenderArgs>(extraArgs));
}

class InvalidAppenderArgsException : public std::length_error
{
public:
	explicit InvalidAppenderArgsException(std::string const& message) : std::length_error(message) { }
};
