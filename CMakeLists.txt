cmake_minimum_required(VERSION 3.12)
project(EternalTextureCompressor)

set(CMAKE_C_STANDARD 99)
set (CMAKE_C_FLAGS "-O2 -DNDEBUG")

add_executable(EternalTextureCompressor compressor.c oodle.h)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        target_link_libraries(EternalTextureCompressor ${CMAKE_SOURCE_DIR}/lib/oo2core_win64.lib)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        target_link_libraries(EternalTextureCompressor ${CMAKE_DL_LIBS} ${CMAKE_SOURCE_DIR}/lib/liboo2corelinux64.a)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        target_link_libraries(EternalTextureCompressor ${CMAKE_DL_LIBS} ${CMAKE_SOURCE_DIR}/lib/liboo2coremac64.a)
endif()
