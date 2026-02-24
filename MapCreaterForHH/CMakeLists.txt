cmake_minimum_required(VERSION 3.10)
project(MapCreaterForHH)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к raylib
set(RAYLIB_PATH "C:/NikDevMSVC/My_MVSC_Projects/CppDestroyer_msvc16/vendor/raylib-5.5_win64_msvc16")
include_directories("${RAYLIB_PATH}/include")
link_directories("${RAYLIB_PATH}/lib")

add_executable(MapCreaterForHH main.cpp)

target_link_libraries(MapCreaterForHH
        raylib
        winmm
        gdi32
        user32
        kernel32
        opengl32
)

# Копируем assets
add_custom_command(TARGET MapCreaterForHH POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:MapCreaterForHH>/assets"
)

# Создаем папку contours
add_custom_command(TARGET MapCreaterForHH POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:MapCreaterForHH>/contours"
)