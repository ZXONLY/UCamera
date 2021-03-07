//
// Created by bytedance on 05/03/2021.
//
#pragma once

#include <IUGraph.h>
#include <map>
#include <mutex>
#include "UGraphSession.h"

namespace core {

    class UGraph : public IUGraph, public std::enable_shared_from_this<IUGraph> {
    public:
        virtual int getUnitCount() override;

        virtual UResult getUnitByIndex() override;

        virtual UResult addUnit() override;

        virtual UResult connectUnit(IUUnit *upstream, StreamID outStreamID,
                                    IUUnit *downstream, StreamID inputStreamID) override;

        virtual UResult clear() override;


    public:

        virtual UResult setThreadPool(IUThreadPool *pool);

        virtual IUThreadPool *getThreadPool();

        //virtual UResult setListener(SessionListener *listener);

        virtual UResult init(void *param);

        virtual UResult ready(void *param = nullptr);

        virtual UResult pause();

        virtual UResult run();

        virtual UResult stop();

        virtual UResult destroy();

        virtual UResult unInit(bool sync);

        virtual UResult setState(GraphState state);

        virtual GraphState getState();

        virtual UResult trySetState(GraphState state);

        GraphState getLastState();

    public:
        virtual UResult setInterrupt(UInterruptID interruptId, UInterruptResponser responser);

        virtual bool
        processInterrupt(UInterruptID iID = INTERRUPT_OPENGL_DRAW, bool autoRemovve = true);

    public:
        virtual void setExtendTime(int64_t addition);

        virtual int64_t getExtendTime();


    protected:
        std::map<UnitID, std::shared_ptr<IUUnit>> _units;

        std::shared_ptr<IUThreadPool> _threadPool;
        //SessionListener *_listener;
        //StreamingProcessor *_streamingProcessor;
        //PipelineResource *_pipelineResource;

        std::atomic<GraphState> m_State;
        std::mutex m_StateLock;
        std::atomic<GraphState> m_lastState;

        int64_t extemdTime_ = 0LL;

        std::mutex _interruptLock;
        std::map<UInterruptID, UInterruptResponser> _interrupts;

    };

}//namespace core
