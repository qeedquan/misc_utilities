cmake_minimum_required(VERSION 3.10)
project(qtgl VERSION 1.0 LANGUAGES CXX)

set(OpenGL_GL_PREFERENCE "GLVND")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically run moc, uic, and rcc when parsing files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include the current directory to locate generated UI headers (for CMake < 3.7)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the required Qt5 components (Core, Gui, and Widgets are standard for desktop apps)
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Define your source files (include .cpp, .h, .ui, and .qrc files)
set(SOURCES
    main.cpp
	widget.cpp
    # mainwindow.cpp      # Uncomment when you add these
    # mainwindow.h
    # mainwindow.ui
)

# Define the executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the Qt5 targets to your executable
target_link_libraries(${PROJECT_NAME}
	PRIVATE
	Qt5::Widgets
	Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    OpenGL::GL
    GLEW::GLEW
)
