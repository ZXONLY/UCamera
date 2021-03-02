//
// Created by bytedance on 02/03/2021.
//

#pragma once

namespace engine{
    class URecordThreadPool{
    public:
        static URecordThreadPool *createThreadPool() {return new URecordThreadPool();};



    };
}
