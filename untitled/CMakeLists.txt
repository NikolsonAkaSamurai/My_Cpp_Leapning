cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Charts  # ← ДОБАВИТЬ ДЛЯ ГРАФИКОВ!
        REQUIRED
)

add_executable(untitled
        main.cpp
        vacancy.cpp
        vacancy.h
        hhparser.cpp
        hhparser.h
        Controller.h
        Controller.cpp
        MainWindow.h
        mainwindow.cpp
)

target_link_libraries(untitled
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Charts  # ← ДОБАВИТЬ ДЛЯ ГРАФИКОВ!
)