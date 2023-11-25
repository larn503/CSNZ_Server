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

target_sources(gui PRIVATE "gui.cpp")
target_sources(gui PRIVATE "mainwindow.ui")
target_sources(gui PRIVATE "mainwindow.cpp")
target_sources(gui PRIVATE "maintab.cpp")
target_sources(gui PRIVATE "maintab.ui")
target_sources(gui PRIVATE "consoletab.cpp")
target_sources(gui PRIVATE "consoletab.ui")
target_sources(gui PRIVATE "sessiontab.cpp")
target_sources(gui PRIVATE "sessiontab.ui")
target_sources(gui PRIVATE "roomlisttab.cpp")
target_sources(gui PRIVATE "roomlisttab.ui")
target_sources(gui PRIVATE "noticedialog.cpp")
target_sources(gui PRIVATE "noticedialog.ui")
target_sources(gui PRIVATE "selectuserdialog.cpp")
target_sources(gui PRIVATE "selectuserdialog.ui")
target_sources(gui PRIVATE "usercharacterdialog.cpp")
target_sources(gui PRIVATE "usercharacterdialog.ui")
target_sources(gui PRIVATE "userbanlistdialog.cpp")
target_sources(gui PRIVATE "userbanlistdialog.ui")
target_sources(gui PRIVATE "hwidbanlistdialog.cpp")
target_sources(gui PRIVATE "hwidbanlistdialog.ui")
target_sources(gui PRIVATE "ipbanlistdialog.cpp")
target_sources(gui PRIVATE "ipbanlistdialog.ui")
target_sources(gui PRIVATE "bandialog.cpp")
target_sources(gui PRIVATE "bandialog.ui")

target_sources(gui PRIVATE "../common/utils.cpp")

target_include_directories(gui PUBLIC
	"../"
)