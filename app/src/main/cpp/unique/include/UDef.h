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


