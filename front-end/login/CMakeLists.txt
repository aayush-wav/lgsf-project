cmake_minimum_required(VERSION 3.16)
project(login VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt components - added Core for QProcess and other core functionality
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql)

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

    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET login APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(login SHARED
            ${PROJECT_SOURCES}
            ${RESOURCE_FILES}
        )
        # Define properties for Android with Qt 5 after find_package() calls as:
        #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(login
            ${PROJECT_SOURCES}
            ${RESOURCE_FILES}
        )
    endif()
endif()

# Link Qt libraries - added Core for QProcess, QCryptographicHash, etc.
target_link_libraries(login PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
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

# Print build information
message(STATUS "Building login application with Qt${QT_VERSION_MAJOR}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Optional: Add debug information for development
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build - additional logging enabled")
    target_compile_definitions(login PRIVATE DEBUG_BUILD)
endif()

# Find PostgreSQL
find_package(PostgreSQL REQUIRED)

# Link Qt SQL module
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# Link libraries
target_link_libraries(your_target_name
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    ${PostgreSQL_LIBRARIES}
)

# Include directories
target_include_directories(your_target_name PRIVATE
    ${PostgreSQL_INCLUDE_DIRS}
)

# If using .pro file instead, add these lines:
# QT += core widgets sql
# LIBS += -lpq
