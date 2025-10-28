cmake_minimum_required(VERSION 3.25.1)

project(ShinyEngine LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

set(SRC_FILES
	src/vulkan_api/utils/Helpers.cpp
	src/vulkan_api/wrappers/setup/VulkanApi.cpp
	src/vulkan_api/wrappers/presentation/MainView.cpp
	src/vulkan_api/wrappers/pipeline/stages/shader/ShaderStage.cpp
	src/vulkan_api/wrappers/pipeline/stages/vertex/VertexInputState.cpp
	src/vulkan_api/wrappers/pipeline/stages/uniform/DescriptorSetLayout.cpp
	src/vulkan_api/wrappers/pipeline/descriptors/DescriptorPool.cpp
	src/vulkan_api/wrappers/pipeline/GraphicsPipeline.cpp
	src/vulkan_api/wrappers/command_pool/CommandBufferPool.cpp
	src/vulkan_api/wrappers/sync/SyncManager.cpp
	src/vulkan_api/wrappers/texture/Texture2D.cpp
	src/vulkan_api/resources/VkResourceHolder.cpp
	src/vulkan_api/wrappers/render/Render.cpp
	src/Application.cpp
	src/main.cpp
)

set(HDR_FILES
	src/Application.hpp
	src/Camera.hpp
	src/vulkan_api/resources/VkResourceHolder.hpp
	src/vulkan_api/utils/Defines.hpp
	src/vulkan_api/utils/Helpers.hpp
	src/vulkan_api/wrappers/command_pool/CommandBufferPool.hpp
	src/vulkan_api/wrappers/pipeline/descriptors/DescriptorPool.hpp        
	src/vulkan_api/wrappers/pipeline/GraphicsPipeline.hpp
	src/vulkan_api/wrappers/pipeline/stages/shader/ShaderStage.hpp
	src/vulkan_api/wrappers/pipeline/stages/uniform/DescriptorSetLayout.hpp
	src/vulkan_api/wrappers/pipeline/stages/vertex/VertexInputState.hpp    
	src/vulkan_api/wrappers/presentation/MainView.hpp
	src/vulkan_api/wrappers/render/Render.hpp
	src/vulkan_api/wrappers/setup/VulkanApi.hpp
	src/vulkan_api/wrappers/sync/SyncManager.hpp
	src/vulkan_api/wrappers/texture/Texture2D.hpp
)

set(SHADER_FILES
	${PROJECT_SOURCE_DIR}/src/shaders/vertex_shader.vert
	${PROJECT_SOURCE_DIR}/src/shaders/fragment_shader.frag
)

source_group("shaders" FILES ${SHADER_FILES})

set(ALL_SRC_FILES
	${SRC_FILES} 
	${HDR_FILES}
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
include(${CMAKE_SOURCE_DIR}/cmake/compile_shaders.cmake)
compile_shaders(${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/shaders)

add_compile_definitions(DEBUG=$<CONFIG:Debug> GLM_FORCE_INTRINSICS GLM_FORCE_EXPLICIT_CTOR)

if(WIN32)
	add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR GLFW_EXPOSE_NATIVE_WIN32)
endif()

if(UNIX)
	add_compile_definitions(VK_USE_PLATFORM_XCB_KHR GLFW_EXPOSE_NATIVE_X11)
endif()

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HDR_FILES})
target_sources(${PROJECT_NAME} PRIVATE ${SHADER_FILES})

add_subdirectory(${EXTERNAL_SOURCE_DIR}/glfw glfw)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/glm glm)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories(${PROJECT_NAME} PRIVATE   
	${Vulkan_INCLUDE_DIRS}
	${EXTERNAL_SOURCE_DIR}/stb
	${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw glm)

if(UNIX)
	target_link_libraries(${PROJECT_NAME} PRIVATE xcb)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res     $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/res/shaders
	VERBATIM
)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_SRC_FILES})
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

if(CMAKE_GENERATOR MATCHES "Visual Studio")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res     ${CMAKE_CURRENT_BINARY_DIR}/res 
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/res/shaders
		VERBATIM
	)
endif()