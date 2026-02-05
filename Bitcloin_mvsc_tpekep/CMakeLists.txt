cmake_minimum_required(VERSION 3.20)
project(Bitcloin_mvsc_tpekep VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Настройка для Qt ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# --- Поиск пакета Qt6 ---
# Ищем все необходимые нам компоненты
find_package(Qt6 6.5 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
        Charts
)

# --- Создание исполняемого файла ---
# Указываем все наши исходные файлы
add_executable(Bitcloin_mvsc_tpekep
        main.cpp
        pricetrackerwindow.h
        pricetrackerwindow.cpp
)

# --- Линковка с библиотеками Qt ---
# Здесь мы связываем нашу программу с найденными компонентами Qt
target_link_libraries(Bitcloin_mvsc_tpekep PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Charts
)
