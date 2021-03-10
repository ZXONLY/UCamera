////
//// Created by bytedance on 05/03/2021.
////
//
//#ifndef CAMERA_UMEDIAENGINE_H
//#define CAMERA_UMEDIAENGINE_H
//
//#endif //CAMERA_UMEDIAENGINE_H
//
//
//#pragma once
//
//#include <map>
//#include <base/include/TERenderFrameSetting.h>
//
//#include "TEGlobal.h"
//#include "TEEngineMonitor.h"
//
//#include "utils/TEBundle.h"
//#include "TEEngineBuilder.h"
//#include "TEEngineContext.h"
//#include "TERecordContext.h"
//
//#include "graph/imp/TEGraph.h"
//#include "TERecorderThreadObject.h"
//#include "TEMediaEngineState.h"
//#include "TERecordThreadPool.h"
//
//using namespace core;
//
//namespace te_engine {
//    TEInterface ITEControllerInterface;
//
//    class TEMediaEngine : public TERecorderThreadObject, public TEEngineContext {
//        friend TEInterface ITEControllerInterface;
//
//        friend TEInterface ITEEngineBuilder;
//
//    public:
//        TEMediaEngine();
//
//        ~TEMediaEngine();
//
//        TEResult setController(ITEControllerInterface *pController, ITEMediaEngineState *pEngineStateController);
//
//        TEResult init(te_recorder::TERecorderContext *recorderContext);
//
//        TEResult start();
//
//        TEResult pause();
//
//        TEResult pauseSync();
//
//        TEResult stop();
//
//        TEResult stopSync();
//
//        TEResult destroy();
//
//        //--------------------------
//
//    public:
//        std::shared_ptr<TEGraph> getGraph();
//
//        TEResult setGraph(std::shared_ptr<TEGraph> pGraph);
//
//        std::shared_ptr<TEEngineMonitor> getMonitor();
//
//        std::shared_ptr<TEEngineMonitorClient> getEngineMonitor();
//
//        TEResult setMonitor(std::shared_ptr<TEEngineMonitor> pMonitor);
//
//        std::shared_ptr<TEEngineContext> getEngineContext();
//
//        std::shared_ptr<TEMediaEngine> getSharedThis();
//
//        TEResult setDisplaySurface(EGLNativeDisplayType surface);
//
//        TEResult setDisplaySettings(const te_recorder::TEDisplaySettings &displaySettings);
//
//        TEResult changeRenderSize(int width, int height);
//
//        TEResult clearDisplay(int color);
//
//        TEResult setLandscape(int rotate, int transX, int transY);
//
//        TEResult enableEngineMonitorReport(bool enable);
//        /**
//         * @brief info report by MessageClient
//         * @param type Info type
//         * @param ext  info externed
//         * @param f
//         */
//        void onInfo(int type, int ext, float f) override;
//
//        /**
//         * @brief error report by MessageClient
//         * @param type
//         * @param ext
//         * @param f
//         * @param msg
//         */
//        void onError(int type, int ext, float f, std::string &msg) override;
//
//        void postOnRenderThread(int type, int ext, float f);
//
//    protected:
//        TEResult _init(TEMsg *pMsg);
//
//        TEResult _start(TEMsg *pMsg);
//
//        TEResult _pause(TEMsg *pMsg);
//
//        TEResult _stop(TEMsg *pMsg);
//
//        TEResult _destroy(TEMsg *pMsg);
//
//        virtual void getThreadAttr(TEThreadAttr *attr) override;
//
//        virtual TEResult customEvent(TEMsg *e) override;
//
//    protected:
//        void _onCallback(int type, int ext, float f, std::string &msg);
//
//        //inner utils methods
//        void _onError(int type, int ext, float f, std::string &msg);
//
//        void _onInfo(int type, int ext, float f);
//
//        int _setEngineState(TEEngineState state);
//
//        int _setEngineState(TEEngineState state, std::string &strMsg);
//
//        void _restoreMKID(int id, char &a, char &b, char &c, int &d);
//
//    private:
//        void updateBin();
//
//    private:
//        std::shared_ptr<TEGraph> m_zGraph;
//        std::shared_ptr<TEEngineMonitor> m_zMonitor;
//        ITEControllerInterface *m_pController = nullptr;
//
//        std::atomic<bool> m_bInitDone, m_bStopDone, m_bPauseDone, m_bDestroyDone;
//
//        // cache param and setting
//        EGLNativeDisplayType m_pSurface = nullptr;
//        unique_ptr<te_recorder::TEDisplaySettings> m_pDisplaySettingCache;
//        unique_ptr<TESizei> m_pRenderSize;
//        int m_iLandscapeRotate = INT32_MIN;
//        int m_iLandscapeTransX = 0;
//        int m_iLandscapeTransY = 0;
//
//        int64_t m_i64EngineInitStartTime = 0;
//
//        std::recursive_mutex operations_recursive_lock_;
//#define OPERATION_RECURSIVE_AUTO_LOCK std::lock_guard<std::recursive_mutex> guard(operations_recursive_lock_);
//#define CHECK_ENGINE_STATE(STATE)                                        \
//    bool checkResult = true;                                             \
//    if (!m_b##STATE##Done) {                                             \
//        VELOGW("please call this func when MediaEngine is##STATE##ed!"); \
//        checkResult = false;                                             \
//    }
//    };
//}
