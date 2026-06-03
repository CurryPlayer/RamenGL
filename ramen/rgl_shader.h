#ifndef RGL_SHADER_H
#define RGL_SHADER_H

#include <glad/glad.h>

#include <stdio.h>
#include <string.h> /* for memset */

#include "rgl_platform.h"

class Shader
{
  public:
    bool Load(const char* vertShaderFile, const char* fragShaderFile);

    void Use();

    void Delete();

  private:
    bool CompileShader(const File& shaderFile, GLuint* shader, GLenum shaderType)
    {
        /* Read shader source from disk and compile. */
        *shader = glCreateShader(shaderType);
        glShaderSource(*shader, 1, &shaderFile.data, nullptr);
        glCompileShader(*shader);
        if ( !IsCompiled(*shader) )
        {
            return false;
        }

        return true;
    }

    bool IsCompiled(GLuint shader)
    {
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if ( status != GL_TRUE )
        {
            char buffer[ 512 ];
            memset(buffer, 0, 512);
            glGetShaderInfoLog(shader, 511, nullptr, buffer);
            printf("GLSL compile error:\n%s\n", buffer);

            return false;
        }

        return true;
    }

  private:
    GLuint m_Program;
};

#endif
