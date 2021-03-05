//
// Created by bytedance on 05/03/2021.
//
#pragma once

#include "UBaseFrame.h"
#include "GLHolder.h"

namespace uq_base{
    class UCameraFrame : public UBaseFrame{
    public:
        virtual ~UCameraFrame();

    protected:
        GLuint m_nTexId;
        GLint m_nInternalFormat;

        int8_t *data = nullptr;
    };
}
