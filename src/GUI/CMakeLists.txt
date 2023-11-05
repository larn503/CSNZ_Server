cmake_minimum_required(VERSION 3.20)
#set(PROJECTNAME "CSO Server GUI")

project(gui CXX)

# required by QT
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(gui STATIC)

target_link_libraries(gui Qt6::Widgets)

target_sources(gui PRIVATE "GUI.cpp")
target_sources(gui PRIVATE "MainWindow.ui")
target_sources(gui PRIVATE "MainWindow.cpp")
target_sources(gui PRIVATE "MainTab.cpp")
target_sources(gui PRIVATE "MainTab.ui")
target_sources(gui PRIVATE "ConsoleTab.cpp")
target_sources(gui PRIVATE "ConsoleTab.ui")
target_sources(gui PRIVATE "SessionTab.cpp")
target_sources(gui PRIVATE "SessionTab.ui")
target_sources(gui PRIVATE "RoomListTab.cpp")
target_sources(gui PRIVATE "RoomListTab.ui")
target_sources(gui PRIVATE "NoticeDialog.cpp")
target_sources(gui PRIVATE "NoticeDialog.ui")

target_sources(gui PRIVATE "../Common/Utils.cpp")

target_include_directories(gui PUBLIC
	"../"
	"../Common"
)