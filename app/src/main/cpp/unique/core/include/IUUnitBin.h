//
// Created by 张翔 on 2/16/21.
//
#pragma once
#include "IUUnit.h"
#include <vector>

namespace core{

class IUUnitBin : public IUUnit{
protected:
        std::vector<IUUnit> m_units;

};

}//end namespace core
