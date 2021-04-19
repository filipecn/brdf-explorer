# avoid policy warning message
if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)
# main project
project(BRDF_EXPLORER)
cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)
set(CMAKE_VERBOSE_MAKEFILE ON)
# path variables
set(ROOT_PROJECT_PATH "${PROJECT_SOURCE_DIR}")
set(BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
###############################################################################
##                                options                                    ##
###############################################################################
#option(BUILD_ALL "build all targets" OFF)
# cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
# check for specific machine/compiler options.
include(ConfigCompilerFlags)
###############################################################################
##                              dependencies                                 ##
###############################################################################
include(ExternalProject)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ext")
include(circe)
###############################################################################
##                                 sources                                   ##
###############################################################################
set(SOURCES brdf_explorer.cpp)
add_executable(brdf_explorer ${SOURCES})
target_compile_definitions(brdf_explorer PUBLIC
        -DSRC_PATH="${CMAKE_CURRENT_SOURCE_DIR}"
        )
add_dependencies(brdf_explorer circe)
target_include_directories(brdf_explorer PUBLIC ${PONOS_INCLUDES} ${CIRCE_INCLUDES} ${JSON11_INCLUDES})
target_link_libraries(brdf_explorer ${CIRCE_LIBRARIES} ${PONOS_LIBRARIES} ${JSON11_LIBRARIES})

