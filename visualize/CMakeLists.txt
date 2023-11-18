CMAKE_MINIMUM_REQUIRED(VERSION 3.23)
PROJECT(img-process)

SET(CMAKE_CXX_STANDARD 11)

# 可执行程序名称，下面统一使用
SET(my_app_name simple-image-process)

# 指定头文件查找目录
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/fltk-dist-1.3.8/include")


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../src/core
	${CMAKE_CURRENT_SOURCE_DIR}/../src/io
	${CMAKE_CURRENT_SOURCE_DIR}/app/myWidgets
	${CMAKE_CURRENT_SOURCE_DIR}/app)


# 将所有头文件路径保存至 all_head_files 数组变量中
# 将所有源文件路径保存至 all_source_files 数组变量中
FILE(GLOB_RECURSE all_source_files "../src/*.cpp" "app/*.cpp" "app/myWidgets/*.cpp")
# 将测试程序移除
list(FILTER all_source_files EXCLUDE REGEX ".*test.cpp$")

FILE(GLOB_RECURSE all_head_files "../src/*h" "app/*.h" "app/myWidgets/*.h")

# 指定库文件查找路径，根据操作系统区分：
IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Windows 操作系统，需要查找 Windows-release
    MESSAGE(STATUS "current platform: Windows")
    LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/fltk-dist-1.3.8/lib/release")
    ADD_EXECUTABLE(
            ${my_app_name}
            WIN32 # Windows 非命令行程序
            ${all_head_files}
            ${all_source_files}
    )
    TARGET_LINK_LIBRARIES(
            ${my_app_name}
            PRIVATE
            fltk # 链接的静态库名称，这里只需要写fltk，在运行时自动查找.a/.lib
    )
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # macOS 操作系统，查找 macOS-release
    MESSAGE(STATUS "current platform: macOS")
    LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/fltk-dist-1.3.8/lib/release")
    ADD_EXECUTABLE(
            ${my_app_name}
            ${all_head_files}
            ${all_source_files}
    )
    TARGET_LINK_LIBRARIES(
            ${my_app_name}
            PRIVATE
            fltk # 链接的静态库名称，这里只需要写fltk，在运行时自动查找.a/.lib
            
            # macOS 该选项必填
            "-framework Cocoa"
    )
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    # Linux 操作系统，查找 Linux-release
    MESSAGE(STATUS "current platform: Linux ")

    LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/fltk-dist-1.3.8/lib/release")
    ADD_EXECUTABLE(
            ${my_app_name}
            ${all_head_files}
            ${all_source_files}
    )
    TARGET_LINK_LIBRARIES(
            ${my_app_name}
            PRIVATE
            fltk # 链接的静态库名称，这里只需要写fltk，在运行时自动查找.a/.lib
            X11 # Linux环境需要指定X11以及dl两个库才能正常显示
            dl
    )
ELSE ()
    # 其他操作系统
    MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF ()