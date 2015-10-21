#pragma once

#include <string>
#include "Appender.h"

enum ColorTypes
{
	BLACK,
	RED,
	GREEN,
	BROWN,
	BLUE,
	MAGENTA,
	CYAN,
	GREY,
	YELLOW,
	LRED,
	LGREEN,
	LBLUE,
	LMAGENTA,
	LCYAN,
	WHITE
};

const uint8 MaxColors = uint8(WHITE) + 1;

class AppenderConsole : public Appender
{
public:
	typedef std::integral_constant<AppenderType, APPENDER_CONSOLE>::type TypeIndex;

	AppenderConsole(uint8 _id, std::string const& name, LogLevel level, AppenderFlags flags, ExtraAppenderArgs extraArgs);
	void InitColors(const std::string& init_str);
	AppenderType getType() const override { return TypeIndex::value; }

private:
	void SetColor(bool stdout_stream, ColorTypes color);
	void ResetColor(bool stdout_stream);
	void _write(LogMessage const* message) override;
	bool _colored;
	ColorTypes _colors[MaxLogLevels];
};

