cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0074 NEW)

project(frontend VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)
find_package(PostgreSQL REQUIRED)

add_subdirectory(login)
add_subdirectory(signup)