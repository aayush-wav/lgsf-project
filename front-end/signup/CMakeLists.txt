cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0074 NEW)

project(signup VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

string(REGEX MATCH "^[0-9]+" QT_VERSION_MAJOR "${QT_VERSION}")
message(STATUS "Detected Qt major version: ${QT_VERSION_MAJOR}")

find_package(PostgreSQL REQUIRED)

# Create dbhandler library (matching your login project structure)
add_library(dbhandler STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../login/dbhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../login/dbhandler.h
)
target_include_directories(dbhandler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../login
)
target_link_libraries(dbhandler PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    ${PostgreSQL_LIBRARY}
)
message(STATUS "DBHandler source path: ${CMAKE_CURRENT_SOURCE_DIR}/../login/dbhandler.cpp")

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

set(RESOURCE_FILES "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/image.qrc")
    list(APPEND RESOURCE_FILES image.qrc)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons.qrc")
    list(APPEND RESOURCE_FILES icons.qrc)
endif()

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(signup
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${RESOURCE_FILES}
    )
else()
    if(ANDROID)
        add_library(signup SHARED
            ${PROJECT_SOURCES}
            ${RESOURCE_FILES}
        )
    else()
        add_executable(signup
            ${PROJECT_SOURCES}
            ${RESOURCE_FILES}
        )
    endif()
endif()

target_include_directories(signup PRIVATE
    ${PostgreSQL_INCLUDE_DIR}
    ../login
)

target_link_libraries(signup PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    ${PostgreSQL_LIBRARY}
    dbhandler
)

if(QT_VERSION VERSION_LESS "6.1.0")
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.signup)
endif()

set_target_properties(signup PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS signup
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(signup)
endif()

message(STATUS "Building signup application with Qt${QT_VERSION_MAJOR}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build - additional logging enabled")
    target_compile_definitions(signup PRIVATE DEBUG_BUILD)
endif()