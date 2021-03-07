//
// Created by 张翔 on 3/6/21.
//

#pragma once

namespace core{
    typedef enum {
        GRAPH_STATE_ANY = 0xffff,
        GRAPH_STATE_ERROR = 0,
        GRAPH_STATE_NOTHING = 1 << 20,

        GRAPH_STATE_IDLE = 1,                     // 1
        GRAPH_STATE_INITIALIZED = 1 << 1,         // 2
        GRAPH_STATE_PREPARED = 1 << 2,            // 4
        GRAPH_STATE_STARTED = 1 << 3,             // 8
        GRAPH_STATE_PAUSED = 1 << 4,              // 16
        GRAPH_STATE_SEEKING = 1 << 5,             // 32
        GRAPH_STATE_STOPPED = 1 << 6,             // 64
        GRAPH_STATE_COMPLETED = 1 << 7,           // 128
        GRAPH_STATE_CAPTURE_RECORDING = 1 << 8,   // 256
        GRAPH_STATE_COMPILING = 1 << 9            // 512
    } GraphState;
}
