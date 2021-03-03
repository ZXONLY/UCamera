//
// Created by bytedance on 02/03/2021.
//

#pragma once

#include "../../include/UDef.h"
#include "../../include/UResult.h"
#include <functional>


//using IUTask = std::function<void()>;
using IUTask = std::function<void()>;

UInterface IUThread{
    virtual UResult syncTask(IUTask task) = 0;
    virtual UResult asyncTask(IUTask task) = 0;
    virtual UResult runTask(IUTask task) = 0;
};

UInterface IUThreadPool{
    virtual UResult creatThread(IUThread **pthread) = 0;

    virtual int checkThreadCount() = 0;

    virtual int recycle() = 0;
};