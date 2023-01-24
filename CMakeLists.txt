cmake_minimum_required(VERSION 3.12)
project(EternalTextureCompressor)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
        set(CMAKE_CXX_FLAGS "/O2 /Oi /Ot /GL /EHsc")
else()
        set (CMAKE_CXX_FLAGS "-O2 -DNDEBUG")
endif()

add_executable(EternalTextureCompressor compressor.cpp ooz.hpp)

if(MSVC)
        target_link_options(EternalTextureCompressor PUBLIC "/LTCG")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        target_link_libraries(EternalTextureCompressor ${CMAKE_SOURCE_DIR}/lib/ooz.lib)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        target_link_libraries(EternalTextureCompressor ${CMAKE_DL_LIBS} ${CMAKE_SOURCE_DIR}/lib/libooz.a)
endif()
