#pragma once

#ifdef DB_SQLITE
#include "userdatabase_sqlite.h"
#elif defined DB_MYSQL
#include "userdatabase_mysql.h"
#elif defined DB_POSTGRESQL
#include "userdatabase_postgresql.h"
#else
#include "userdatabase_keyvalues.h"
#endif

#ifdef DB_SQLITE
extern class CUserDatabaseSQLite* g_pUserDatabase;
#elif defined DB_MYSQL
extern class CUserDatabaseMySQL* g_pUserDatabase;
#elif defined DB_POSTGRESQL
extern class CUserDatabasePostgreSQL* g_pUserDatabase;
#else
extern class CUserDatabase* g_pUserDatabase;
#endif

