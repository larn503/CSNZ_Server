set(BUILD_SHARED_LIBS OFF)

add_subdirectory(KeyValues)
add_subdirectory(json)
add_subdirectory(wolfssl)

if (CSO_SERVER_DBSQLITE)
	set(SQLITECPP_USE_STATIC_RUNTIME OFF CACHE BOOL "Use MSVC static runtime (default for internal googletest).")
	add_subdirectory(SQLiteCpp)
endif()
