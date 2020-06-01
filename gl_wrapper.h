#ifndef __GL_WRAPPER_H__
#define __GL_WRAPPER_H__

#define OPENGL_DEBUG 1

#if OPENGL_DEBUG
#define gl(x, ...) {\
        gl##x(__VA_ARGS__);\
        GLenum err = glGetError(); \
        switch(err) {\
        case GL_INVALID_ENUM: printf("[%s(%d), %s]: GL_INVALID_ENUM\n", __FILE__, __LINE__, #x); break; \
        case GL_INVALID_VALUE: printf("[%s(%d), %s]: GL_INVALID_VALUE\n", __FILE__, __LINE__, #x); break; \
        case GL_INVALID_OPERATION: printf("[%s(%d), %s]: GL_INVALID_OPERATION\n", __FILE__, __LINE__, #x); break; \
        case GL_INVALID_FRAMEBUFFER_OPERATION: printf("[%s(%d), %s]: GL_INVALID_FRAMEBUFFER_OPERATION\n", __FILE__, __LINE__, #x); break; \
        case GL_OUT_OF_MEMORY: printf("[%s(%d), %s]: GL_OUT_OF_MEMORY\n", __FILE__, __LINE__, #x); break; \
        case GL_STACK_UNDERFLOW: printf("[%s(%d), %s]: GL_STACK_UNDERFLOW\n", __FILE__, __LINE__, #x); break; \
        case GL_STACK_OVERFLOW: printf("[%s(%d), %s]: GL_STACK_OVERFLOW\n", __FILE__, __LINE__, #x); break; \
        case GL_NO_ERROR: break;\
        default: break;}}
#else
#define gl(x, ...) gl##x(__VA_ARGS__);
#endif

#endif