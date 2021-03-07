cmake_minimum_required(VERSION 3.4.1)

include_directories(
        ${CMAKE_SOURCE_DIR}/inc
        ${CMAKE_SOURCE_DIR}/thrid-party/include
        ${CMAKE_SOURCE_DIR}/OpenGLUtils/include

        ${CMAKE_SOURCE_DIR}/unique/base/include
        ${CMAKE_SOURCE_DIR}/unique/camera/include
        ${CMAKE_SOURCE_DIR}/unique/core/include
        ${CMAKE_SOURCE_DIR}/unique/core/impl/include
        ${CMAKE_SOURCE_DIR}/unique/engine/include
        ${CMAKE_SOURCE_DIR}/unique/include
        ${CMAKE_SOURCE_DIR}/unique/common/include

#        ${CMAKE_SOURCE_DIR}/base/include
#        ${CMAKE_SOURCE_DIR}/base/glutils/include
)

include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/ffmpeg/include)


aux_source_directory(src SRCLIST)
aux_source_directory(thrid-party THRIDPARTY)
aux_source_directory(OpenGLUtils openGL

aux_source_directory(unique/core CORE)
aux_source_directory(unique/core/impl COREIMPL)
aux_source_directory(unique/base BASE)
aux_source_directory(unique/camera CAMERA)
aux_source_directory(unique/engine ENGINE)
aux_source_directory(unique/common COMMON)



MESSAGE( STATUS "this var key = ${SRCLIST}.")

add_library( # Specifies the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${SRCLIST}
        ${THRIDPARTY}
        ${openGL}

        ${CORE}
        ${CAMERA}
        ${BASE}
        ${ENGINE}
        ${COREIMPL}
        ${COMMON}

        )

find_library( # Defines the name of the path variable that stores the
        # location of the NDK library.
        log-lib

        # Specifies the name of the NDK library that
        # CMake needs to locate.
        log )
# Links your native library against one or more other native libraries.

# 设置ffmpeg库所在路径的目录
set(distribution_DIR ${CMAKE_SOURCE_DIR}/thrid-party/${ANDROID_ABI})

add_library( avutil
        SHARED
        IMPORTED )
set_target_properties( avutil
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavutil.so)

add_library( swresample
        SHARED
        IMPORTED )
set_target_properties( swresample
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libswresample.so)

add_library( avfilter
        SHARED
        IMPORTED )
set_target_properties( avfilter
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavfilter.so)

add_library( avformat
        SHARED
        IMPORTED )
set_target_properties( avformat
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavformat.so)

add_library( swscale
        SHARED
        IMPORTED )
set_target_properties( swscale
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libswscale.so)

add_library( avcodec
        SHARED
        IMPORTED )
set_target_properties( avcodec
        PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/libavcodec.so)


target_link_libraries( # Specifies the target library.

        native-lib

        avutil
        swresample
        avfilter
        avformat
        swscale
        avcodec

        GLESv2
        EGL
        android
        # Links the log library to the target library.
        ${log-lib} )