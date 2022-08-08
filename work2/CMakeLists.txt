#项目名称
project(work2)
#代码路径
aux_source_directory(. DIR_TOOT_SRCS)
#dubug 模式
set (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
#生成可执行的文件
add_executable(work2 ${DIR_TOOT_SRCS})