# CMake最低版本号要求
cmake_minimum_required(VERSION 3.0)

# 项目信息
project(QingLite)

#查找当前目录下的所有源文件
#并将名称保存到DIR_SRCS变量
aux_source_directory(. DIR_SRCS)

#添加src子目录
add_subdirectory(src)
add_subdirectory(basic/unix)
add_subdirectory(basic/algorithm)

#指定生成目标 
add_executable(QingLite main.cpp ${DIR_SRCS})

# 添加链接库
target_link_libraries(QingLite SRCLib BasicAlgorithm BasicUNIX)
