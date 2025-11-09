#命令、关键字不区分大小写；变量区分大小写
cmake_minimum_required(VERSION 3.15) # 指定 CMake 最低版本（必须放在最前面）
cmake_policy(SET CMP0167 OLD) # 将策略 CMP0167 设置为使用旧版本的行为。
#cmake_policy(SET CMP0091 NEW)
project("ffmepeg-base-server" # CMAKE_PROJECT_NAME
        VERSION 1.0.0
        DESCRIPTION "A cross-platform app"
        HOMEPAGE_URL agustletmen.github.io
        LANGUAGES C CXX
)


set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

# 强制要求指定标准，不满足则报错
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 禁用编译器扩展（保证跨平台兼容性）
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_BUILD_TYPE Release)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif ()
message("Build type: " ${CMAKE_BUILD_TYPE})

include(cmake/env.cmake)
# print_all_variables()# 使用宏
custom_print_variables()# 使用宏



file(GLOB SRC_CODE
 "${PROJECT_SOURCE_DIR}/include/*.h"
 "${PROJECT_SOURCE_DIR}/include/*.hpp"
 "${PROJECT_SOURCE_DIR}/src/*.cpp"
 "${PROJECT_SOURCE_DIR}/src/*.cxx"
 "${PROJECT_SOURCE_DIR}/src/*.c"
 "${PROJECT_SOURCE_DIR}/src/*.cc"
)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_CODE})


# target_include_directories(目标名 作用域 路径1 路径2 ...)
target_include_directories(${CMAKE_PROJECT_NAME}
        PRIVATE # 仅当前目标生效（内部头文件，不对外暴露）
        ${PROJECT_SOURCE_DIR}/include
        #        PUBLIC   # 当前目标+依赖它的目标生效（对外暴露的头文件）
        #        INTERFACE # 仅对外暴露，自身不用
)


# 去哪里找要链接的库文件
target_link_directories(${CMAKE_PROJECT_NAME}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/lib
)

if (CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]$")
    # Debug 模式：链接带 d 后缀的调试库
    set(OpenCV_LIB "opencv_world4120d")
else ()
    # Release 模式（默认）：链接无后缀的发布库
    set(OpenCV_LIB "opencv_world4120")
endif ()

# 要链接哪些具体的库文件
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
     ${OpenCV_LIB}
)

