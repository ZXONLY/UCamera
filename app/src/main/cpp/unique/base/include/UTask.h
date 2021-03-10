//
// Created by bytedance on 09/03/2021.
//
#pragma once

#include <functional>
#include "UPipeline.h"

namespace core{
    typedef struct UTask {

        typedef std::function<void(core::UPipeline *)> PipelineExecutor;

        UTask(core::UPipeline *videoPipeline, PipelineExecutor exe) : pipeline(*videoPipeline), executor(exe) {}

        UTask(core::UPipeline *videoPipeline, PipelineExecutor exe, int Flags) : pipeline(*videoPipeline), executor(exe), flags(Flags) {}

        UTask(PipelineExecutor exe) : executor(exe) {}

        core::UPipeline pipeline;
        PipelineExecutor executor;
        int flags = 0;
    } UTask;

    typedef UTask::PipelineExecutor TEPipelineTask;
}
