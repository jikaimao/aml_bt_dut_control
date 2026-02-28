cmake_minimum_required(VERSION 3.20)

project(aml_bt_dut_control
    VERSION 0.1.0
    DESCRIPTION "Industrial-grade extensible DUT controller for Windows"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_EXAMPLE "Build command-line example" ON)

add_library(dut_controller
    src/DutController.cpp
    src/TransportFactory.cpp
    src/transports/UartTransport.cpp
    src/transports/AdbTransport.cpp
    src/transports/UsbTransport.cpp)

target_include_directories(dut_controller
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(dut_controller
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Werror>)

if(BUILD_EXAMPLE)
    add_executable(dut_controller_example src/main.cpp)
    target_link_libraries(dut_controller_example PRIVATE dut_controller)
endif()

install(TARGETS dut_controller
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(DIRECTORY include/ DESTINATION include)
