#pragma once

#ifdef DB_PROXY
#define REAL_DATABASE_NAME "RealUserDatabase"
#else
#define REAL_DATABASE_NAME "UserDatabase"
#endif
