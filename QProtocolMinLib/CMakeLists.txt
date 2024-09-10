cmake_minimum_required(VERSION 3.14)

project(QProtocolMinLib LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort)

add_library(QProtocolMinLib STATIC
    ContextWrapper.h ContextWrapper.cpp)
target_link_libraries(QProtocolMinLib Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::SerialPort)

add_subdirectory(min)
target_link_libraries(QProtocolMinLib min)

target_compile_definitions(min PRIVATE -DNO_TRANSPORT_PROTOCOL)

target_include_directories(QProtocolMinLib PUBLIC ".")
