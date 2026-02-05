cmake_minimum_required(VERSION 3.16)
project(BtcQtTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Интеграция с Conan ---
# Этот блок остается нашим надежным помощником
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    message(STATUS "Conan: Toolchain not found, running 'conan install'")
    execute_process(
            COMMAND conan install . --output-folder=${CMAKE_BINARY_DIR} --build=missing
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE conan_result
    )
    if(conan_result)
        message(FATAL_ERROR "Conan: 'conan install' failed.")
    endif()
endif()

# Подключаем магию Conan
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# --- Поиск пакетов Qt ---
# Qt требует особого подхода, но он очень логичен
find_package(Qt6 REQUIRED COMPONENTS Core Gui Network)

# Создаем исполняемый файл
add_executable(BtcQtTracker main.cpp
        main.cpp)

# Qt требует специальной обработки для MOC (Meta-Object Compiler)
# Эта команда включает всю магию Qt для нашего проекта.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# --- Линковка библиотек Qt ---
# Просто и элегантно
target_link_libraries(BtcQtTracker PRIVATE Qt::Core Qt::Gui Qt::Network)

# --- Флаги для MinGW ---
if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    message(STATUS "MinGW detected. Applying static linking flags.")
endif()
