cmake_minimum_required(VERSION 3.16)
project(test8 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Allow user override of SFML location; default to the user-provided path
if(NOT DEFINED SFML_DIR)
    set(SFML_DIR "D:/Programme/SFML-3.0.2/lib/cmake/SFML")
endif()

message(STATUS "Using SFML_DIR=${SFML_DIR}")

find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

add_executable(app
    src/main.cpp
    src/tetris/tetromino.cpp
    src/tetris/board.cpp
    src/tetris/game.cpp
)

target_include_directories(app PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(app PRIVATE SFML::Graphics SFML::Window SFML::System)

# Put binary in build/bin
set_target_properties(app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

enable_testing()
add_subdirectory(tests)
