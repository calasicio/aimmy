cmake_minimum_required(VERSION 3.25)

project(aimmy
  VERSION 1.0.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(FetchContent)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_MakeAvailable(json)

add_library(imgui STATIC
  src/vendor/imgui/imgui.cpp
  src/vendor/imgui/imgui_draw.cpp
  src/vendor/imgui/imgui_tables.cpp
  src/vendor/imgui/imgui_widgets.cpp

  src/vendor/imgui/backends/imgui_impl_dx11.cpp
  src/vendor/imgui/backends/imgui_impl_win32.cpp
)

target_include_directories(imgui
  PUBLIC
  src/vendor/imgui
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.hpp" "src/*.h")

# CRITICAL: Remove vendor files from executable sources
list(FILTER SOURCES EXCLUDE REGEX "vendor")
list(FILTER HEADERS EXCLUDE REGEX "vendor")

add_executable(${PROJECT_NAME}
  ${SOURCES}
  ${HEADERS}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
  src
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
  $<$<CONFIG:Release>:/O2 /W4 /WX->
  $<$<CONFIG:Debug>:/W4 /WX- /Zi /Od>
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  nlohmann_json::nlohmann_json
  imgui
  d3d11

  kernel32
  user32
  gdi32
  winspool
  comdlg32
  advapi32
  shell32
  ole32
  oleaut32
  uuid
)

if(MSVC)
  target_link_options(${PROJECT_NAME}
    PRIVATE
    /SUBSYSTEM:CONSOLE
  )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
)
