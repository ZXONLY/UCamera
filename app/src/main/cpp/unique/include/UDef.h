//
// Created by bytedance on 02/03/2021.
//
#pragma once

#include <functional>

#define UInterface struct

typedef enum enumInterruptID{
    INTERRUPT_NONE = 0,
    INTERRUPT_OPENGL_DRAW = 1
} UInterruptID;

using UInterruptResponser = std::function<bool(UInterruptID)>;


using U_THREAD_STATE = enum {
    U_THREAD_STATE_UNINIT = 0x0,
    U_THREAD_STATE_IDLE,
    U_THREAD_STATE_RUNNING,
    U_THREAD_STATE_RUNEND,
    U_THREAD_STATE_ERROR,
};

using U_THREAD_TYPE = enum {
    U_THREAD_TYPE_NORMAL = 0x0,
    U_THREAD_TYPE_GL,
};


//
// Created by glumes on 2020/12/10.
//
//
//#ifndef ANDROID_TERECORDTHREADKERNEL_H
//#define ANDROID_TERECORDTHREADKERNEL_H
//
//#include <iostream>
//
//namespace kernel {
//    enum class TEThreadDesc {
//        GLOBAL_GL_THREAD = 0,
//        MAIN_RENDER_GL_THREAD = 1,
//        VIDEO_ENCODE_GL_THREAD = 2,
//        GET_FRAME_GL_THREAD = 3,
//        NORMAL_WORK_THREAD = 4,
//        SINGLE_TASK_THREAD = 5,
//        DISPLAY_TASK_THREAD = 6
//    };
//
//    struct EnumClassHash {
//        template <typename T> std::size_t operator()(T t) const { return static_cast<std::size_t>(t); }
//    };
//
//}   // namespace kernel
//
//#endif   // ANDROID_TERECORDTHREADKERNEL_H


