cmake_minimum_required(VERSION 3.16)
project(login VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt components
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql)

# Find PostgreSQL
find_package(PostgreSQL REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# Add resource files to sources if they exist
set(RESOURCE_FILES "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/image.qrc")
    list(APPEND RESOURCE_FILES image.qrc)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons.qrc")
    list(APPEND RESOURCE_FILES icons.qrc)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
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

# Link libraries
target_link_libraries(login PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    ${PostgreSQL_LIBRARIES}
)

# Include directories
target_include_directories(login PRIVATE
    ${PostgreSQL_INCLUDE_DIRS}
)

# macOS bundle settings
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.login)
endif()

set_target_properties(login PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS login
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(login)
endif()

# Build information
message(STATUS "Building login application with Qt${QT_VERSION_MAJOR}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Debug mode logging
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build - additional logging enabled")
    target_compile_definitions(login PRIVATE DEBUG_BUILD)
endif()
