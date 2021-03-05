//
// Created by 张翔 on 2/16/21.
//

#pragma once

#include <UDef.h>
#include <UResult.h>
#include "IUUnit.h"

namespace core {
    UInterface IUGraph{

        virtual int getUnitCount() = 0;

        virtual UResult getUnitByIndex() = 0;

        virtual UResult addUnit() = 0;

        virtual UResult connectUnit(IUUnit *upstream, StreamID outStreamID,
                                    IUUnit *downstream, StreamID inputStreamID) = 0;

        virtual UResult clear() = 0;

    };


}
