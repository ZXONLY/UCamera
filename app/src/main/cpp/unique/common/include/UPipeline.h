//
// Created by bytedance on 05/03/2021.
//
#pragma once

#include <memory>
#include <vector>
#include "../base/include/UCameraFrame.h"

namespace core{
    typedef struct UPipelineContext{

    } UPipelineContext;

    typedef struct UPipeline{
        std::shared_ptr<UBaseFrame> frame;
        std::vector<std::shared_ptr<UBaseFrame> > extendFrames;
        UPipelineContext pContext;
    } UPipeline;
}
