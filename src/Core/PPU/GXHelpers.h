#ifndef DSHBA_GXHELPERS_H
#define DSHBA_GXHELPERS_H

#include "default.h"
#include "log.h"

// ----- IMPORTANT -----
// glad must be generated for GLES (gles2 profile w/ 3.2)
// ---------------------
#include <GLES3/gl32.h>

static void CompileShader(GLuint shader, const char* name) {
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        // GLES shader logs can exceed 1 KB
        char infoLog[2048];        
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        //log_fatal("Shader compilation failed (%s): %s\n", name, infoLog);
        log_info("Shader compilation failed (%s): %s\n", name, infoLog);
    }
}

static void LinkProgram(GLuint program, const char* name) {
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[2048];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        log_fatal("Shader program linking failed (%s): %s\n", name, infoLog);
    }
}

static void CheckFramebufferInit(const char* name) {
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                log_fatal("Framebuffer %s incomplete attachment", name);
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                log_fatal("Framebuffer %s missing attachment", name);
            case GL_FRAMEBUFFER_UNSUPPORTED:
                log_fatal("Framebuffer %s unsupported format", name);
            default:
                log_fatal("Framebuffer %s unknown error (0x%x)", name, status);
        }
    }
}

#endif // DSHBA_GXHELPERS_H
