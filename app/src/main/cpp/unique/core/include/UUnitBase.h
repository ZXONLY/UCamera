//
// Created by 张翔 on 3/7/21.
//
#pragma once
#include "IUUnit.h"

namespace core{

    typedef struct UConnectInfo{
        std::shared_ptr<IUUnit> downstreamUnit;
        StreamID inputStreamId;
    } UDownConnectInfo;

    typedef struct {
        std::shared_ptr<IUUnit> upstreamUnit;
        StreamID upStreamId;
    } UUpConnectInfo;


    class UUnitBase : public IUUnit{
    protected:
        UUnitBase();
        virtual ~UUnitBase();

    public:
        virtual UnitID getUnitId() override ;

        virtual UnitType getUnitType() override;

        //动态增删
        virtual UResult addInputStream(StreamID newStreamID) override;

        virtual UResult removeInputStream(StreamID streamId) override;

        //连接Unit
        virtual UResult
        connectTo(StreamID outPutStream, IUUnit *downStream, StreamID inputStreamId) override;

        virtual UResult disconnec(StreamID outputStreamID) override;

        virtual bool isIdentity(UPipeline *pipeline) override;

        //推模式
        virtual UResult processInputStream(StreamID inputStreamId, UPipeline pipeline) override;

        //消息处理
        //virtual UResult postMessage(const TEMsg &msg) = 0;

        //消息处理
        //virtual UResult processMessage(const TEMsg &msg, enum ProcessMode mode) = 0;

        //资源管理
        virtual UResult setThreadPool(IUThreadPool *pool) override;

//        virtual UResult setFrameAllocator(ITEVideoFrameAllocator *allocator) = 0;
//
//        virtual UResult setHostFrameAllocator(ITEVideoFrameAllocator *allocator) = 0;

        //状态控制
        virtual UResult init() override;

        virtual UResult ready() override;

        virtual UResult pause() override;

        virtual UResult run() override;

        virtual UResult stop() override;

        virtual UnitState getState() override;

        virtual void destroy() override;

    private:

    protected:
        UnitType _unitType;
        UnitID _unitId;
        UnitState _unitState;

        std::map<StreamID, UInputStreamInfo> _inputs;
        std::map<StreamID, UOutputStreamInfo> _outputs;

    protected:
        std::map<StreamID, UDownConnectInfo> _connects;
        std::map<StreamID, UUpConnectInfo> _upStreamConnects;

        std::shared_ptr<IUThreadPool> _threadpool;


    };

}// namespace core
