cmake_minimum_required(VERSION 3.16)
project(TetrisGame VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# ------------------------------------------------------------------
# Options
# ------------------------------------------------------------------
option(BUILD_TESTS "Build unit tests" OFF)

# ------------------------------------------------------------------
# Qt
# ------------------------------------------------------------------
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

set(QT_MODULES Core Gui Widgets Sql Multimedia)
if(BUILD_TESTS)
    list(APPEND QT_MODULES Test)
endif()

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_MODULES})
message(STATUS "Using Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")

# ------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------
set(SOURCES
    src/main.cpp
    src/tetrisboard.cpp
    src/tetrisgame.cpp
    src/tetrispiece.cpp
    src/nextpiecewidget.cpp
    src/scoredatabase.cpp
    src/leaderboarddialog.cpp
    src/AudioManager.cpp
)

set(HEADERS
    include/constants.h
    include/tetrisboard.h
    include/tetrisgame.h
    include/tetrispiece.h
    include/nextpiecewidget.h
    include/scoredatabase.h
    include/leaderboarddialog.h
    include/AudioManager.h
)

# ------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Multimedia
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# ------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
