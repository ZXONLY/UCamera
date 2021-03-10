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


