cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0074 NEW)

project(login VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt components (Widgets and Sql)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

# Extract Qt major version number from QT_VERSION string (e.g. "6.9.0")
string(REGEX MATCH "^[0-9]+" QT_VERSION_MAJOR "${QT_VERSION}")
message(STATUS "Detected Qt major version: ${QT_VERSION_MAJOR}")

# Set PostgreSQL root path
set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/17")
find_package(PostgreSQL REQUIRED)

# Project source files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    dbhandler.cpp
    dbhandler.h
)

# Optional resource files
set(RESOURCE_FILES "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/image.qrc")
    list(APPEND RESOURCE_FILES image.qrc)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons.qrc")
    list(APPEND RESOURCE_FILES icons.qrc)
endif()

# Create executable
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(login
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${RESOURCE_FILES}
    )
else()
    if(ANDROID)
        add_library(login SHARED
            ${PROJECT_SOURCES}
            ${RESOURCE_FILES}
        )
    else()
        add_executable(login
            ${PROJECT_SOURCES}
            ${RESOURCE_FILES}
        )
    endif()
endif()

# Link Qt and PostgreSQL libraries
target_link_libraries(login PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    ${PostgreSQL_LIBRARY}
)

# Include PostgreSQL headers
target_include_directories(login PRIVATE
    ${PostgreSQL_INCLUDE_DIR}
)

# macOS bundle settings (safe default, doesn't hurt on Windows)
if(QT_VERSION VERSION_LESS "6.1.0")
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.login)
endif()

# Set application properties
set_target_properties(login PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Install target
include(GNUInstallDirs)
install(TARGETS login
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize Qt6 executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(login)
endif()

# Log build info
message(STATUS "Building login application with Qt${QT_VERSION_MAJOR}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Debug-only definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build - additional logging enabled")
    target_compile_definitions(login PRIVATE DEBUG_BUILD)
endif()
