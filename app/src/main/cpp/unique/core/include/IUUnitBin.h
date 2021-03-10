//
// Created by 张翔 on 2/16/21.
//
#pragma once

#include "IUUnit.h"
#include <vector>
#include <map>

namespace core {

    class IUUnitBin : public IUUnit {
    public:
        virtual UResult
        processInputStream(StreamID inputStreamID, UPipeline *pipeline) override ;

        /**
         * dispatch messages to bin
         * @param msg
         * @return
         */
        virtual UResult postMessage(const UMsg &msg) override ;

        //virtual UResult processMessage(const UMsg &msg, enum ProcessMode mode) override;
    protected:
        void initVideoStreamInfo(int inCount, int outCount);

        UResult pushFrame(UPipeline *pipeline,IUUnit* src = nullptr, StreamID intputID = 0);
        UResult getSinkOutputFrame(UPipeline **outFrame, StreamID outputStreamId = 0);

        std::shared_ptr < IUUnit> &sinkUnit(StreamID outputStreamId = 0);
        std::shared_ptr < IUUnit> &sourceUnit(StreamID inputStreamId = 0);
    protected:
        std::map<UnitID, std::shared_ptr < IUUnit> > units_;

        std::vector<std::shared_ptr<IUUnit>> sourceUnits_;

        std::vector<std::shared_ptr<IUUnit>> sinkUnits_;

    };

}//end namespace core
