# 1. Определение проекта
cmake_minimum_required(VERSION 3.25)
project(CppDestroyer LANGUAGES CXX)

# 2. Настройка стандарта языка
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Определение цели (нашего исполняемого файла)
add_executable(CppDestroyer main.cpp)

# 4. Настройка зависимостей для цели CppDestroyer
target_include_directories(CppDestroyer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib-5.5_win64_msvc16/include
)
target_link_directories(CppDestroyer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib-5.5_win64_msvc16/lib
)
target_link_libraries(CppDestroyer PRIVATE raylib opengl32 gdi32 winmm)

# 5. Копирование ресурсов (картинок, звуков) в папку со сборкой
add_custom_command(
        TARGET CppDestroyer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:CppDestroyer>/assets
        COMMENT "Copying assets directory..."
)
