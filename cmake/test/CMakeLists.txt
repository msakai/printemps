cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

# ##############################################################################
# switch compiler options according to CMAKE_BUILD_TYPE
# ##############################################################################
message(${CMAKE_BUILD_TYPE})

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

# ##############################################################################
# # tests
# ##############################################################################
enable_testing()

link_directories(
    ${TOP_DIR}/external/lib
)

include(test_mps.cmake)
include(test_example.cmake)
include(test_helper.cmake)
include(test_model.cmake)
include(test_model_component.cmake)
include(test_multi_array.cmake)
include(test_neighborhood.cmake)
include(test_option.cmake)
include(test_option_enumerate.cmake)
include(test_opb.cmake)
include(test_wcnf.cmake)
include(test_extra_maxsat_evaluation.cmake)
include(test_preprocess.cmake)
include(test_solution.cmake)
include(test_standalone.cmake)
include(test_utility.cmake)
include(test_utility_graph.cmake)
include(test_utility_json.cmake)
include(test_utility_ucb1.cmake)
include(test_utility_sparse.cmake)

# ##############################################################################
# # END
# ##############################################################################