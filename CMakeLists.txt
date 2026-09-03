set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 3.6)

# the project name is MyGame, rename as needed
project(MyGame CXX)

set(CMAKE_CXX_STANDARD 11)

if(WIN32)
    # SDL3's devel packages ship their own CMake config files (SDL3Config.cmake,
    # SDL3_image-config.cmake, etc). Point CMAKE_PREFIX_PATH at wherever you've
    # extracted each bundled package and let find_package do the rest - no more
    # manually setting each *_LIBRARY / *_INCLUDE_DIR by hand.
    # Adjust these paths to match how you've laid out libs/SDL3* on disk.
    list(APPEND CMAKE_PREFIX_PATH
            "${CMAKE_SOURCE_DIR}/libs/SDL3"
            "${CMAKE_SOURCE_DIR}/libs/SDL3_image"
            "${CMAKE_SOURCE_DIR}/libs/SDL3_mixer"
            "${CMAKE_SOURCE_DIR}/libs/SDL3_ttf")

    find_package(SDL3 REQUIRED CONFIG)
    find_package(SDL3_image REQUIRED CONFIG)
    find_package(SDL3_mixer REQUIRED CONFIG)
    find_package(SDL3_ttf REQUIRED CONFIG)

    # copy dynamic libraries - filenames changed vs SDL2 (e.g. SDL2.dll -> SDL3.dll),
    # and SDL3's devel zip usually keeps DLLs under a bin/ folder rather than lib/x64.
    # Adjust these paths/names to match your actual extracted package layout.
    file(COPY
            "${CMAKE_SOURCE_DIR}/libs/SDL3/lib/x64/SDL3.dll"
            "${CMAKE_SOURCE_DIR}/libs/SDL3_image/lib/x64/SDL3_image.dll"
            "${CMAKE_SOURCE_DIR}/libs/SDL3_mixer/lib/x64/SDL3_mixer.dll"
            "${CMAKE_SOURCE_DIR}/libs/SDL3_ttf/lib/x64/SDL3_ttf.dll"
            DESTINATION ${CMAKE_BINARY_DIR})
else()
    # SDL3, SDL3_image, SDL3_ttf, and SDL3_mixer all provide CMake config
    # packages once installed (e.g. via package manager or `make install`),
    # so no custom Find*.cmake modules are needed here anymore.
    find_package(SDL3 REQUIRED CONFIG)
    find_package(SDL3_image REQUIRED CONFIG)
    find_package(SDL3_mixer REQUIRED CONFIG)
    # find_package(SDL3_ttf REQUIRED CONFIG)
endif()

# load user source and header files
file(GLOB_RECURSE SOURCE_FILES "src/*.h" "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# make assets directory in build
#file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)

# copy game assets into build/assets directory
#file(COPY
#        "assets/assetName.png"
#        DESTINATION ${CMAKE_BINARY_DIR}/assets)

# Copy /res into build directory so that the project has access
file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})

# CMake config targets carry their own include dirs and link requirements,
# so there's no need for a separate include_directories() call anymore.
# SDL3::SDL3main only exists on platforms that need a custom WinMain shim
# (mainly Windows); TARGET_NAME_IF_EXISTS keeps this link line portable.
target_link_libraries(${PROJECT_NAME}
        $<TARGET_NAME_IF_EXISTS:SDL3::SDL3main>
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer
        SDL3_ttf::SDL3_ttf)

# Change VS startup project to MyGame
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MyGame)