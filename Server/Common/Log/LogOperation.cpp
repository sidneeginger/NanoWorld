#include "stdafx.h"

#include "LogOperation.h"
#include "Logger.h"

int LogOperation::call()
{
	logger->write(msg.get());
	return 0;
}
