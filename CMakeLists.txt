cmake_minimum_required(VERSION 3.19)

file(READ "${CMAKE_SOURCE_DIR}/VERSION" VER_RAW)
string(STRIP ${VER_RAW} VER)

project(
    checkpoint
    DESCRIPTION "Tiny reverse proxy that attempts to block AI scrapers"
    VERSION ${VER}
)

set(CHECKPOINT_VERSION ${VER})
set(PISTACHE_BUILD_TESTS OFF)
set(PISTACHE_BUILD_FUZZ OFF)

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
