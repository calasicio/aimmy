cmake_minimum_required(VERSION 3.20)
project(aimmy VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /WX- /Od /Oi /Gy /Zi /EHsc)
else()
    add_compile_options(-O3 -Wall -Wextra)
endif()

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.hpp"
)

include_directories(src)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    kernel32
    user32
    gdi32
    psapi
    wininet
)
