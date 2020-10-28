cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/inc )
#include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/inc/ttffmpeg)

aux_source_directory(src/main/cpp/src SRCLIST)

add_library( # Specifies the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${SRCLIST} )

find_library( # Defines the name of the path variable that stores the
        # location of the NDK library.
        log-lib

        # Specifies the name of the NDK library that
        # CMake needs to locate.
        log )
# Links your native library against one or more other native libraries.
target_link_libraries( # Specifies the target library.
        native-lib
        GLESv2
        # Links the log library to the target library.
        ${log-lib} )