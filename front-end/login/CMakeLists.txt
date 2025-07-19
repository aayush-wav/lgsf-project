cmake_minimum_required(VERSION 3.16)
project(login VERSION 0.1 LANGUAGES CXX)

# Set paths
list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.9.0/mingw_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/login_autogen/include
)

set(PROJECT_SOURCES
    main.cpp
    loginwindow.cpp
    loginwindow.h
    loginwindow.ui
)

# Add resource files to sources if they exist
set(RESOURCE_FILES "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/image.qrc")
    list(APPEND RESOURCE_FILES image.qrc)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons.qrc")
    list(APPEND RESOURCE_FILES icons.qrc)
endif()

# Create executable
qt_add_executable(login
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${RESOURCE_FILES}
)

# Link Qt libraries
target_link_libraries(login PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

# Set Windows specific properties
if(WIN32)
    set_target_properties(login PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Qt finalization
qt_finalize_executable(login)

# Print build information
message(STATUS "Building login application with Qt6")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
