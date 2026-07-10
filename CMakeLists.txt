cmake_minimum_required(VERSION 3.16)
project(TetrisGame VERSION 3.0 LANGUAGES CXX)

# ------------------------------------------------------------------
# C++ standard
# ------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# ------------------------------------------------------------------
# Qt — auto-detect Qt6 or fall back to Qt5
# ------------------------------------------------------------------
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    Multimedia
    Test
)

message(STATUS "Using Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")

# ------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------
set(SOURCES
    main.cpp
    tetrisboard.cpp
    tetrisgame.cpp
    tetrispiece.cpp
    nextpiecewidget.cpp
    scoredatabase.cpp
    leaderboarddialog.cpp
    AudioManager.cpp
)

set(HEADERS
    constants.h
    tetrisboard.h
    tetrisgame.h
    tetrispiece.h
    nextpiecewidget.h
    scoredatabase.h
    leaderboarddialog.h
    AudioManager.h
)

# ------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Multimedia
)

# ------------------------------------------------------------------
# Platform-specific
# ------------------------------------------------------------------
if(WIN32)
    # Prevent a console window from appearing alongside the GUI
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# ------------------------------------------------------------------
# Install
# ------------------------------------------------------------------
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Copy sounds folder next to the binary after build (optional — won't fail if missing)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/sounds"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/sounds"
    COMMAND_EXPAND_LISTS
)

# ------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------
enable_testing()
add_subdirectory(tests)
