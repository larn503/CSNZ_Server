#include <doctest/doctest.h>
#include "../common/logger.h"

using namespace std;

#define TEST_MESSAGE_ARGS "%s %s %d\n", "test1", "test2", 227
#define TEST_MESSAGE "test1 test2 227\n"

#define BUF_SIZE 128

class CTestLogger : public CBaseLogger
{
public:
	CTestLogger() : CBaseLogger(false)
	{
	}

	void LogVarg(int level, const char* msg, va_list argptr)
	{
		CHECK(level == LogLevel::LOG_LEVEL_INFO);

		char buf[BUF_SIZE];
		vsnprintf(buf, sizeof(buf), msg, argptr);

		CHECK(strcmp(buf, TEST_MESSAGE) == 0);
	}
};

TEST_CASE("Logger - test logger")
{
	CTestLogger logger;
	logger.Info(TEST_MESSAGE_ARGS);
}

class CTestLoggerPrefix : public CBaseLogger
{
public:
	CTestLoggerPrefix() : CBaseLogger(false)
	{
	}

	void LogVarg(int level, const char* msg, va_list argptr)
	{
		CHECK(level == LogLevel::LOG_LEVEL_INFO);

		char buf[BUF_SIZE];
		vsnprintf(buf, sizeof(buf), msg, argptr);

		// check if string contains required message, the first part should be prefix
		CHECK(strstr(buf, TEST_MESSAGE) != 0);
	}
};

TEST_CASE("Logger - test logger with prefix")
{
	CLoggerPrefix logger(new CTestLoggerPrefix());
	logger.Info(TEST_MESSAGE_ARGS);
}

class CTestLoggerComposite : public CBaseLogger
{
public:
	CTestLoggerComposite() : CBaseLogger(false)
	{
	}

	void LogVarg(int level, const char* msg, va_list argptr)
	{
		CHECK(level == LogLevel::LOG_LEVEL_INFO);

		// message must be already formatted
		CHECK(strstr(msg, TEST_MESSAGE) != 0);
		CHECK(argptr == NULL);
	}
};

TEST_CASE("Logger - test composite logger")
{
	CCompositeLogger logger(false, { new CTestLoggerComposite() });
	logger.Info(TEST_MESSAGE_ARGS);
}
