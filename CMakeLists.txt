cmake_minimum_required(VERSION 3.19)

# Set CMake policies to avoid warnings
cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 NEW)  # FetchContent_MakeAvailable
endif()

file(READ "${CMAKE_SOURCE_DIR}/VERSION" VER_RAW)
string(STRIP ${VER_RAW} VER)

project(
    checkpoint
    DESCRIPTION "Tiny reverse proxy that attempts to block AI scrapers"
    VERSION ${VER}
)

set(CHECKPOINT_VERSION ${VER})

# Set Pistache options before adding subdirectory
set(PISTACHE_BUILD_TESTS OFF CACHE BOOL "Build Pistache tests" FORCE)
set(PISTACHE_BUILD_FUZZ OFF CACHE BOOL "Build Pistache fuzzing" FORCE)
set(PISTACHE_BUILD_EXAMPLES OFF CACHE BOOL "Build Pistache examples" FORCE)
set(PISTACHE_BUILD_DOCS OFF CACHE BOOL "Build Pistache docs" FORCE)
set(PISTACHE_USE_RAPIDJSON OFF CACHE BOOL "Use RapidJSON" FORCE)

# RapidJSON fix - set minimum version policy
set(CMAKE_POLICY_DEFAULT_CMP0000 NEW)

add_subdirectory(subprojects/pistache)
add_subdirectory(subprojects/fmt)
add_subdirectory(subprojects/tinylates)
add_subdirectory(subprojects/glaze)

file(GLOB_RECURSE SRCFILES CONFIGURE_DEPENDS "src/*.cpp")

set(CMAKE_CXX_STANDARD 23)

add_executable(checkpoint ${SRCFILES})

add_compile_options(-Wno-deprecated-declarations -Wno-deprecated)
add_compile_definitions(CHECKPOINT_VERSION="${CHECKPOINT_VERSION}")

find_package(PkgConfig REQUIRED)

pkg_check_modules(deps IMPORTED_TARGET openssl re2 libmagic)

target_include_directories(checkpoint
PRIVATE
   "./subprojects/pistache/include"
   "./subprojects/glaze/include"
   "./subprojects/tinylates/include"
   "./subprojects/pistache/subprojects/cpp-httplib"
   "./subprojects/pistache/subprojects/hinnant-date/include"
)
target_link_libraries(checkpoint
    PkgConfig::deps
    pistache
    fmt
    tinylates
)

# Learning/Debug-friendly options
option(CHECKPOINT_ENABLE_SANITIZERS "Enable Address/Undefined sanitizers (Debug only)" ON)
option(CHECKPOINT_STRICT_WARNINGS "Enable -Wall -Wextra -Werror in Debug" ON)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    if(CHECKPOINT_STRICT_WARNINGS)
        add_compile_options(-Wall -Wextra -Werror)
    endif()
    if(CHECKPOINT_ENABLE_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            # Extra useful sanitizers with Clang
            add_compile_options(-fsanitize=float-divide-by-zero,nullability)
            add_link_options(-fsanitize=float-divide-by-zero,nullability)
        endif()
    endif()
endif()
