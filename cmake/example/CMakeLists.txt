cmake_minimum_required(VERSION 3.15)

project(printemps)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED)

if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

option(LINK_STATIC OFF)

# ##############################################################################
# switch compiler options according to CMAKE_BUILD_TYPE
# ##############################################################################
if(MSVC)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        add_compile_options(/W3 /O2 /permissive- /utf-8 /bigobj /EHsc /D_USE_MATH_DEFINES /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4267 /wd4305 /wd4996 /wd4056 /wd4756 /wd4129 /wd5051)
    else() # Debug
        add_compile_options(/W3 /Od /Zi /permissive- /utf-8 /bigobj /EHsc /D_USE_MATH_DEFINES /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4267 /wd4305 /wd4996 /wd4056 /wd4756 /wd4129 /wd5051)
    endif()
else()
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        add_compile_options(
            -Wall
            -O3
            -Wextra
            -Wno-error=missing-field-initializers
            -pthread
        )

        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
            # for Apple Silicon
            add_compile_options(-mcpu=native)
        else()
            # for x86_64
            add_compile_options(-march=native -mtune=native)
        endif()
    else() # Debug
        add_compile_options(
            -Wall
            -Wextra
            -Wno-error=missing-field-initializers
            -pthread
        )
    endif()
endif()

# ##############################################################################
# executable
# ##############################################################################

# simple_1
add_executable(
    simple_1 ${TOP_DIR}/example/simple_1.cpp
)

target_include_directories(
    simple_1
    PRIVATE ${TOP_DIR}/printemps/
)

# simple_2
add_executable(
    simple_2 ${TOP_DIR}/example/simple_2.cpp
)

target_include_directories(
    simple_2
    PRIVATE ${TOP_DIR}/printemps/
)

# knapsack
add_executable(
    knapsack ${TOP_DIR}/example/knapsack.cpp
)

target_include_directories(
    knapsack
    PRIVATE ${TOP_DIR}/printemps/
)

# bin_packing
add_executable(
    bin_packing ${TOP_DIR}/example/bin_packing.cpp
)

target_include_directories(
    bin_packing
    PRIVATE ${TOP_DIR}/printemps/
)

# sudoku
add_executable(
    sudoku ${TOP_DIR}/example/sudoku.cpp
)

target_include_directories(
    sudoku
    PRIVATE ${TOP_DIR}/printemps/
)

if(LINK_STATIC)
    if(MSVC)
        set_property(TARGET simple_1    PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET simple_2    PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET knapsack    PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET bin_packing PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET sudoku      PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_link_options(simple_1 PRIVATE -static-libgcc -static-libstdc++ -static)
        target_link_options(simple_2 PRIVATE -static-libgcc -static-libstdc++ -static)
        target_link_options(knapsack PRIVATE -static-libgcc -static-libstdc++ -static)
        target_link_options(bin_packing PRIVATE -static-libgcc -static-libstdc++ -static)
        target_link_options(sudoku PRIVATE -static-libgcc -static-libstdc++ -static)
    endif()
endif()

# ##############################################################################
# END
# ##############################################################################
