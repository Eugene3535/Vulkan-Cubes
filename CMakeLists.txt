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
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

set(PIPELINE_STAGES_FILES
	src/vulkan_api/wrappers/pipeline/stages/shader/ShaderStage.cpp
	src/vulkan_api/wrappers/pipeline/stages/vertex/VertexInputState.cpp
	src/vulkan_api/wrappers/pipeline/stages/uniform/DescriptorSetLayout.cpp
)

set(SRC_FILES
	src/vulkan_api/utils/Helpers.cpp
	src/vulkan_api/wrappers/api/VulkanApi.cpp
	src/vulkan_api/wrappers/view/MainView.cpp
	${PIPELINE_STAGES_FILES}
	src/vulkan_api/wrappers/pipeline/GraphicsPipeline.cpp
	src/vulkan_api/wrappers/command_pool/CommandBufferPool.cpp
	src/vulkan_api/wrappers/sync/SyncManager.cpp
	src/vulkan_api/wrappers/texture/Texture2D.cpp
	src/vulkan_api/wrappers/mesh/Mesh.cpp
	src/vulkan_api/wrappers/uniform/UniformBufferHolder.cpp
	src/Application.cpp
	src/main.cpp
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
include(${CMAKE_SOURCE_DIR}/cmake/compile_shaders.cmake)
compile_shaders(${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/shaders)

add_compile_definitions(DEBUG=$<CONFIG:Debug>)

if(WIN32)
	add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR GLFW_EXPOSE_NATIVE_WIN32)
endif()

if(UNIX)
	add_compile_definitions(VK_USE_PLATFORM_XCB_KHR GLFW_EXPOSE_NATIVE_X11)
endif()

add_executable(${PROJECT_NAME} ${SRC_FILES})

add_subdirectory(${EXTERNAL_SOURCE_DIR}/glfw glfw)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/glm glm)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories(${PROJECT_NAME} PRIVATE   
	${Vulkan_INCLUDE_DIRS}
	${EXTERNAL_SOURCE_DIR}/stb
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw glm xcb)

if(UNIX)
	target_link_libraries(${PROJECT_NAME} PRIVATE xcb)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res     $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/res/shaders
	VERBATIM
)