cmake_minimum_required(VERSION 3.20)
project(HH_Parser_V3)

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
        Charts
        REQUIRED
)

# Заголовочные файлы
set(HEADERS
        core/Point.h
        core/Vacancy.h
        core/HHParser.h
        core/Controller.h
        core/Contour.h
        core/GeoCalculator.h
        core/IOContours.h
        core/Geocoder.h
        core/GeocoderWorker.h
        modules/ProfileManager.h
        modules/ChartManager.h
        modules/FilterManager.h
        modules/MapManager.h
        modules/MapWidget.h
        modules/SortingManager.h
        MainWindow.h
)

# Исходные файлы
set(SOURCES
        main.cpp
        modules/ProfileManager.cpp
        modules/ChartManager.cpp
        modules/FilterManager.cpp
        modules/MapManager.cpp
        modules/MapWidget.cpp
        modules/SortingManager.cpp
        core/GeocoderWorker.cpp        # ← добавить!
)

add_executable(HH_Parser_V3
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(HH_Parser_V3
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Charts
)

# Копируем контуры в папку сборки
add_custom_command(TARGET HH_Parser_V3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data/contours"
        "$<TARGET_FILE_DIR:HH_Parser_V3>/data/contours"
        COMMENT "Copying contours to build directory"
)

# Создаем папки для данных
add_custom_command(TARGET HH_Parser_V3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:HH_Parser_V3>/data/profiles"
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:HH_Parser_V3>/data/filters"
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:HH_Parser_V3>/data/cache"
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:HH_Parser_V3>/data/contours"
        COMMENT "Creating data directories"
)