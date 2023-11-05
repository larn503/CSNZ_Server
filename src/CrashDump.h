#pragma once

#include "Windows.h"

#ifdef _WIN32
LONG __stdcall ExceptionFilter(EXCEPTION_POINTERS* pep);
#endif
