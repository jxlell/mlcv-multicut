cmake_minimum_required(VERSION 3.10)
project(MulticutCompression)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-aware compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-O3 -g -fcolor-diagnostics -fansi-escape-codes)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-O3 -g -Wall -Wextra)
elseif(MSVC)
    add_compile_options(/O2 /Zi /W4)
endif()

# -------------------------------
# Find OpenCV
# -------------------------------
find_package(OpenCV REQUIRED)

# -------------------------------
# Your source files (manually listed)
# -------------------------------
set(PROJECT_SOURCES
    code/main.cpp
    code/compress.cpp
    code/decompress.cpp
    code/huffman.cpp
    code/Image.cpp
    code/mc.cpp
    code/Util.cpp
)

# -------------------------------
# zlib source (from zlib-1.3.1/)
# -------------------------------
set(ZLIB_SOURCES
    zlib-1.3.1/adler32.c
    zlib-1.3.1/compress.c
    zlib-1.3.1/crc32.c
    zlib-1.3.1/deflate.c
    zlib-1.3.1/infback.c
    zlib-1.3.1/inffast.c
    zlib-1.3.1/inflate.c
    zlib-1.3.1/inftrees.c
    zlib-1.3.1/trees.c
    zlib-1.3.1/uncompr.c
    zlib-1.3.1/zutil.c
)

add_library(zlib STATIC ${ZLIB_SOURCES})
target_include_directories(zlib PUBLIC zlib-1.3.1)

# -------------------------------
# Final executable
# -------------------------------
add_executable(multicut_compression ${PROJECT_SOURCES})
target_include_directories(multicut_compression PRIVATE code)
target_link_libraries(multicut_compression PRIVATE zlib ${OpenCV_LIBS})
target_include_directories(multicut_compression PRIVATE ${OpenCV_INCLUDE_DIRS})
