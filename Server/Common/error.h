#pragma once
#include "define.h"

//namespace Trinity
//{
//	DECLSPEC_NORETURN void Assert(char const* file, int line, char const* function, char const* message) ATTR_NORETURN;
//	DECLSPEC_NORETURN void Assert(char const* file, int line, char const* function, char const* message, char const* format, ...) ATTR_NORETURN ATTR_PRINTF(5, 6);
//
//	DECLSPEC_NORETURN void Fatal(char const* file, int line, char const* function, char const* message) ATTR_NORETURN;
//
//	DECLSPEC_NORETURN void Error(char const* file, int line, char const* function, char const* message) ATTR_NORETURN;
//
//	void Warning(char const* file, int line, char const* function, char const* message);
//
//} // namespace Trinity
//
//#if COMPILER == COMPILER_MICROSOFT
//#define ASSERT_BEGIN __pragma(warning(push)) __pragma(warning(disable: 4127))
//#define ASSERT_END __pragma(warning(pop))
//#else
//#define ASSERT_BEGIN
//#define ASSERT_END
//#endif
//
//#define WPAssert(cond, ...) ASSERT_BEGIN do { if (!(cond)) Trinity::Assert(__FILE__, __LINE__, __FUNCTION__, #cond, ##__VA_ARGS__); } while(0) ASSERT_END
//#define WPFatal(cond, msg) ASSERT_BEGIN do { if (!(cond)) Trinity::Fatal(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
//#define WPError(cond, msg) ASSERT_BEGIN do { if (!(cond)) Trinity::Error(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
//#define WPWarning(cond, msg) ASSERT_BEGIN do { if (!(cond)) Trinity::Warning(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
//
//#define ASSERT WPAssert

