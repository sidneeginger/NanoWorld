#include "stdafx.h"
#include "Appender.h"
#include "Common.h"
#include "Util.h"
#include "StringFormat.h"

#include <utility>
#include <sstream>

std::string LogMessage::getTimeStr(time_t time)
{
	tm aTm;
	localtime_r(&time, &aTm);
	char buf[20];
	snprintf(buf, 20, "%04d-%02d-%02d_%02d:%02d:%02d", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
	return std::string(buf);
}

std::string LogMessage::getTimeStr()
{
	return getTimeStr(mtime);
}

Appender::Appender(uint8 _id, std::string const& _name, LogLevel _level /* = LOG_LEVEL_DISABLED */, AppenderFlags _flags /* = APPENDER_FLAGS_NONE */) :
	id(_id), name(_name), level(_level), flags(_flags) { }

Appender::~Appender() { }

uint8 Appender::getId() const
{
	return id;
}

std::string const& Appender::getName() const
{
	return name;
}

LogLevel Appender::getLogLevel() const
{
	return level;
}

AppenderFlags Appender::getFlags() const
{
	return flags;
}

void Appender::setLogLevel(LogLevel _level)
{
	level = _level;
}

void Appender::write(LogMessage* message)
{
	if (!level || level > message->level)
		return;

	std::ostringstream ss;

	if (flags & APPENDER_FLAGS_PREFIX_TIMESTAMP)
		ss << message->getTimeStr() << ' ';

	if (flags & APPENDER_FLAGS_PREFIX_LOGLEVEL)
		ss << StringFormat("%-5s ", Appender::getLogLevelString(message->level));

	if (flags & APPENDER_FLAGS_PREFIX_LOGFILTERTYPE)
		ss << '[' << message->type << "] ";

	message->prefix = ss.str();
	_write(message);
}

const char* Appender::getLogLevelString(LogLevel level)
{
	switch (level)
	{
	case LOG_LEVEL_FATAL:
		return "FATAL";
	case LOG_LEVEL_ERROR:
		return "ERROR";
	case LOG_LEVEL_WARN:
		return "WARN";
	case LOG_LEVEL_INFO:
		return "INFO";
	case LOG_LEVEL_DEBUG:
		return "DEBUG";
	case LOG_LEVEL_TRACE:
		return "TRACE";
	default:
		return "DISABLED";
	}
}
