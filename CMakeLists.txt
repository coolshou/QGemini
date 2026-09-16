cmake_minimum_required(VERSION 3.16)
project(qgemini VERSION 1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    WebEngineCore
    WebEngineWidgets
    DBus
)

set(SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/webpage.h
    src/webpage.cpp
    src/config.h
    src/config.cpp
    src/i18n.h
    src/resources.qrc
)

add_executable(qgemini ${SOURCES})

target_link_libraries(qgemini PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineCore
    Qt6::WebEngineWidgets
    Qt6::DBus
)

include(GNUInstallDirs)
install(TARGETS qgemini DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES usr/share/applications/qgemini.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES usr/share/icons/hicolor/24x24/apps/qgemini.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/24x24/apps)
