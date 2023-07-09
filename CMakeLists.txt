cmake_minimum_required(VERSION 3.12)
project(EchoFirstGLFWAPP)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# 添加源代码文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 添加头文件路径
include_directories(include)

# 将可执行文件放置在与资源文件夹同一级的位置
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 添加静态库路径
#link_directories(lib-x86_64)


message("zjzjzj CMAKE_CURRENT_BINARY_DIR is ${CMAKE_CURRENT_BINARY_DIR}")
message(EXECUTABLE_OUTPUT_PATH)

message(WARNING "This is a warning message")
#message(FATAL_ERROR "This is an error message")
message(STATUS "This is a debug message")

# 链接静态库
#target_link_libraries(${PROJECT_NAME} 
#SOIL 
#glfw3 
#GLEW
#)


target_link_libraries(${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_SOURCE_DIR}/lib-x86_64/libGLEW.a
    ${CMAKE_SOURCE_DIR}/lib-x86_64/libSOIL.a
    ${CMAKE_SOURCE_DIR}/lib-x86_64/libglfw3.a
    OpenGL::GL
    "-framework CoreVideo"
    "-framework IOKit"
    "-framework Cocoa"
)


# 添加资源文件
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
set(RESOURCES_FILES "${RESOURCES_DIR}/container.jpg")

#add_custom_target(CopyResources ALL
    #COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/Resources
    #COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCES_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Resources
    #COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCES_SHADER_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Resources
    #COMMENT "Copying resources zzz"
    #COMMENT ${CMAKE_CURRENT_BINARY_DIR}
#)

# 复制Resources文件夹到构建目录下
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources
        ${CMAKE_BINARY_DIR}/Resources
    COMMENT "Copying Resources folder"
)
#add_dependencies(${PROJECT_NAME} CopyResources)


