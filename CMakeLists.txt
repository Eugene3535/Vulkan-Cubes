cmake_minimum_required(VERSION 3.25.1)

project(ShinyEngine LANGUAGES C CXX)

set(EXTERNAL_SOURCE_DIR $ENV{External})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

if(MSVC)
	string(APPEND CMAKE_RUNTIME_OUTPUT_DIRECTORY "/bin")
	string(APPEND CMAKE_LIBRARY_OUTPUT_DIRECTORY "/lib")
	string(APPEND CMAKE_ARCHIVE_OUTPUT_DIRECTORY "/lib")
else()
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

set(BUILD_SHARED_LIBS ON)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "Enable tests" FORCE)

find_package(Vulkan REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

set(SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
	# ${CMAKE_SOURCE_DIR}/src/Application.cpp
	${CMAKE_SOURCE_DIR}/src/VulkanApi.cpp
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
include(${CMAKE_SOURCE_DIR}/cmake/compile_shaders.cmake)
compile_shaders(${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/shaders)

add_compile_definitions("DEBUG=$<CONFIG:Debug>")

add_executable(${PROJECT_NAME} ${SRC_FILES})

add_subdirectory(${EXTERNAL_SOURCE_DIR}/glfw glfw)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/glm glm)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories(${PROJECT_NAME} PRIVATE   
	${Vulkan_INCLUDE_DIRS}
	${EXTERNAL_SOURCE_DIR}/stb
)

target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES} glfw glm)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res     $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/res/shaders
	VERBATIM
)