project(test)

add_executable(test)

include_directories(PRIVATE
	"../"
	"../public"
	"../common"
	"../thirdparty"
	"../thirdparty/doctest"
	"../thirdparty/json/include"
)

add_subdirectory(net)

target_sources(test PRIVATE "testserver.cpp")
target_sources(test PRIVATE "testmanager.cpp")
target_sources(test PRIVATE "../manager/manager.cpp")
target_sources(test PRIVATE "../common/console.cpp")

#target_sources(test PRIVATE "testquestmanager.cpp")
#target_sources(test PRIVATE "../manager/questmanager.cpp")

