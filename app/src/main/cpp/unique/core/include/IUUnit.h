//
// Created by 张翔 on 2/17/21.
//
#pragma once

#include <UDef.h>
#include <UPipeline.h>
#include <UResult.h>
#include <IUThreadPool.h>

namespace core {
    typedef int UnitID;
    typedef int StreamID;

    typedef enum UnitType {
        UNIT_TYPE_SOURCE,
        UNIT_TYPE_PROCESSOR,
        UNIT_TYPE_SINK,
        UNIT_TYPE_BIN
    } UnitType;

    typedef struct {
        int streamType; //audio,video
        union {//TODO
//            UVideoStreamInfo video;
//            UAudioStreamInfo audio;
        };
    } AVStreamInfo;

    typedef struct {
        AVStreamInfo avInfo;
        bool fixedSampleSize;   //每帧大小是否固定，若固定，则可从av取值
        bool newThread;         //是否在新增线程，用于评估资源
        int maxLatency;        //要buffer的最长时间导致的延时
        long flag;              //是否可移除等flag, & 方法
    } UInputStreamInfo;

    typedef UInputStreamInfo UOutputStreamInfo;

    typedef enum UnitState {
        UNIT_STATE_INITED,
        UNIT_STATE_STOPPED,
        UNIT_STATE_READY,
        UNIT_STATE_PAUSED,
        UNIT_STATE_RUNNING,
    } UnitState;


    UInterface IUUnit {
        virtual UnitID getUnitId() = 0;

        virtual UnitType getUnitType() = 0;

        //动态增删
        virtual UResult addInputStream(StreamID newStreamID) = 0;

        virtual UResult removeInputStream(StreamID streamId) = 0;

        //连接Unit
        virtual UResult
        connectTo(StreamID outPutStream, IUUnit *downStream, StreamID inputStreamId) = 0;

        virtual UResult disconnec(StreamID outputStreamID) = 0;

        virtual bool isIdentity(UPipeline *pipeline) = 0;

        //推模式
        virtual UResult processInputStream(StreamID inputStreamId, UPipeline pipeline) = 0;

        //消息处理
        //virtual UResult postMessage(const TEMsg &msg) = 0;

        //消息处理
        //virtual UResult processMessage(const TEMsg &msg, enum ProcessMode mode) = 0;

        //资源管理
        virtual UResult setThreadPool(IUThreadPool *pool) = 0;

//        virtual UResult setFrameAllocator(ITEVideoFrameAllocator *allocator) = 0;
//
//        virtual UResult setHostFrameAllocator(ITEVideoFrameAllocator *allocator) = 0;

        //状态控制
        virtual UResult init() = 0;

        virtual UResult ready() = 0;

        virtual UResult pause() = 0;

        virtual UResult run() = 0;

        virtual UResult stop() = 0;

        virtual UnitState getState() = 0;

        virtual void destroy(){};

        virtual ~IUUnit(){}
    };

}//end namespace core