cmake_minimum_required(VERSION 3.10)

# CMAKE_BUILD_TYPE requires Ninja or Makefiles
if (NOT ${CMAKE_GENERATOR} STREQUAL "Ninja")
	message(FATAL_ERROR "Only Ninja generator is supported")
endif()

# Set the project name and version
project(auto_graph VERSION 1.0)

# Debug filename postfix used in CPython
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(PYTHON_DEBUG_POSTFIX "_d")
endif()

# Find Python dirs
set(PYTHON_ROOT_DIR    ${CMAKE_SOURCE_DIR}/external/cpython) # set this to the path to your Python installation
set(Python_INCLUDE_DIR ${PYTHON_ROOT_DIR}/Include ${PYTHON_ROOT_DIR}/PC)
set(PYTHON_BUILD_DIR   ${PYTHON_ROOT_DIR}/PCBuild/amd64)
set(PYTHON_LIB_FILE    ${PYTHON_BUILD_DIR}/python312${PYTHON_DEBUG_POSTFIX}.lib)
message(${PYTHON_LIB_FILE})

# CPython build commands
add_custom_command(
	OUTPUT ${PYTHON_LIB_FILE}
	COMMAND ${PYTHON_ROOT_DIR}/PCbuild/build.bat -p x64 -c ${CMAKE_BUILD_TYPE}
	COMMENT "Building Release CPython ..."
	WORKING_DIRECTORY ${PYTHON_ROOT_DIR}
	VERBATIM
)

# CPython lib target
add_custom_target(PYTHON_LIB DEPENDS ${PYTHON_LIB_FILE})

# Add the executable target
add_executable(${PROJECT_NAME})

# Add source files
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/auto_graph/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})

# Include Python headers
target_include_directories(${PROJECT_NAME} PRIVATE ${Python_INCLUDE_DIR})

# Link with Python library
add_dependencies(${PROJECT_NAME} PYTHON_LIB)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PYTHON_LIB})

# Set the C++ standard to C++20
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)