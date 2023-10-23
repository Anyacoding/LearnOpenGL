cmake_minimum_required(VERSION 3.20)

project(LearnOpenGL)

# C++标准
set(CMAKE_CXX_STANDARD 20)

# 头文件目录
include_directories(dependent/include src)

# 第三方库目录
link_directories(dependent/lib)

# 创建目标
add_executable(main)

# 通过更改 which 来选择运行对应的文件
set(which src/1.getting_started/1.3.3.shaders_class/*.cpp)

# 递归搜索文件并自动更新
file(GLOB_RECURSE source CONFIGURE_DEPENDS  ${which} dependent/src/*.cpp dependent/src/*.c)

# 添加源文件
target_sources(main PRIVATE ${source})

# 链接第三方库
target_link_libraries(main opengl32 glfw3)

# 使用Release版本
#SET(CMAKE_BUILD_TYPE "Release")

