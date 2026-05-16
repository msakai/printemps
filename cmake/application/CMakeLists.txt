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
    else() # Debug
        add_compile_options(
            -Wall
            -Wextra
            -Wno-error=missing-field-initializers
            -pthread
        )
    endif()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../common/CpuArch.cmake)

# ##############################################################################
# executable
# ##############################################################################
add_executable(printemps ${TOP_DIR}/application/standalone/main.cpp)
add_executable(mps_solver ${TOP_DIR}/application/standalone/main.cpp)
add_executable(opb_solver ${TOP_DIR}/application/standalone/main.cpp)

target_include_directories(
    printemps
    PRIVATE ${TOP_DIR}/printemps/
    PRIVATE ${TOP_DIR}/external/include/
)

target_include_directories(
    mps_solver
    PRIVATE ${TOP_DIR}/printemps/
    PRIVATE ${TOP_DIR}/external/include/
)

target_include_directories(
    opb_solver
    PRIVATE ${TOP_DIR}/printemps/
    PRIVATE ${TOP_DIR}/external/include/
)

if(LINK_STATIC)
    if(MSVC)
        set_property(TARGET printemps  PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET mps_solver PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET opb_solver PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_link_options(printemps PRIVATE -static-libgcc -static-libstdc++ -static)
        target_link_options(mps_solver PRIVATE -static-libgcc -static-libstdc++ -static)
        target_link_options(opb_solver PRIVATE -static-libgcc -static-libstdc++ -static)
    endif()
endif()

# ##############################################################################
# END
# ##############################################################################
