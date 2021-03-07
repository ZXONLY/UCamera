//
// Created by bytedance on 02/03/2021.
//
#pragma once

#include "GLHolder.h"

class UFrameBuffer{
public:
    UFrameBuffer(){
        glGenBuffers(1,&m_framebuffer);
    }
    ~UFrameBuffer(){
        glDeleteFramebuffers(1, &m_framebuffer);
        m_framebuffer = 0;
    }

    inline void bind()const {
        glBindFramebuffer(GL_FRAMEBUFFER,m_framebuffer);
    }

    inline void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    inline void unbindTexture2D() const {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    inline bool bindTexture2D(GLuint texID, GLenum attachment = GL_COLOR_ATTACHMENT0) const {
        bind();
        glBindTexture(GL_TEXTURE_2D, texID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texID, 0);
        return true;
    }

    inline bool bindTexture2D(GLuint texID, GLsizei x, GLsizei y, GLsizei w, GLsizei h,
                              GLenum attachment = GL_COLOR_ATTACHMENT0) const {
        bool ret = true;
        ret = bindTexture2D(texID, attachment);
        glViewport(x, y, w, h);
        return ret;
    }

    inline GLuint getID() { return m_framebuffer; }

private:
    GLuint m_framebuffer;
};