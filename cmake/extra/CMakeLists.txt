cmake_minimum_required(VERSION 3.10)

project(printemps)

find_package(OpenMP REQUIRED)

if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

option(LINK_STATIC OFF)

# ##############################################################################
# switch compiler options according to CMAKE_BUILD_TYPE
# ##############################################################################
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(
        -std=c++17
        -Wall
        -O3
        -Wextra
        -Wno-error=missing-field-initializers
        -pthread
    )

else() # Debug
    add_compile_options(
        -std=c++17
        -Wall
        -Wextra
        -Wno-error=missing-field-initializers
        -pthread
    )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../common/CpuArch.cmake)

# ##############################################################################
# executable
# ##############################################################################

# pb_solver for Pseudo-Boolean Competition 2025
add_executable(
    pb_competition_2025_solver ${TOP_DIR}/extra/pb_competition_2025/main.cpp
)

target_include_directories(
    pb_competition_2025_solver
    PRIVATE ${TOP_DIR}/printemps/
    PRIVATE ${TOP_DIR}/external/include/
)

if(LINK_STATIC)
    target_link_options(pb_competition_2025_solver PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

# ##############################################################################
# END
# ##############################################################################
