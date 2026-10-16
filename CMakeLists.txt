
cmake_minimum_required(VERSION 3.20)

project(aether_core VERSION 1.0.0 LANGUAGES CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND DEFINED ENV{HOME})
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local" CACHE PATH "Install prefix" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(aether
    src/terminal_ui.cpp
    src/system_info.cpp
    src/network.cpp
    src/reports.cpp
)

target_include_directories(aether PUBLIC include)

target_compile_options(aether PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
)

add_executable(aether-core main.cpp)
target_link_libraries(aether-core PRIVATE aether)
set_target_properties(aether-core PROPERTIES OUTPUT_NAME AetherCore)

include(CTest)

if(BUILD_TESTING)
    add_executable(aether-tests tests/test_main.cpp)
    target_link_libraries(aether-tests PRIVATE aether)
    add_test(NAME aether-tests COMMAND aether-tests)
endif()

install(TARGETS aether-core RUNTIME DESTINATION bin)

