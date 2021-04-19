set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE "ON")

if (CMAKE_COMPILER_IS_GNUCXX)
    set(DEBUG_FLAGS "${DEBUG_FLAGS} -g -pg -Wall -Wextra -O0 -fprofile-arcs -ftest-coverage --coverage -fPIC")
    set(RELEASE_FLAGS "-O3 -fPIC")
endif (CMAKE_COMPILER_IS_GNUCXX)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(DEBUG_FLAGS "-std=c++17 -g -Wall -Wextra -fPIC")
    set(RELEASE_FLAGS "-std=c++17 -O3 -Wall -Wextra  -fPIC")
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

if (MSVC)
    add_definitions(-DNOMINMAX)
    set(DEBUG_FLAGS "/FS /Zi /EHsc /MDd /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")
    set(RELEASE_FLAGS "/FS /EHsc")
endif (MSVC)

set(CMAKE_CXX_FLAGS ${RELEASE_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_CONFIGURATION_TYPES Debug Release)