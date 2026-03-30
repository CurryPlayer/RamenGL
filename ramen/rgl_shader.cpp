#include "rgl_shader.h"
#include "ramen.h"
#include "rgl_filesystem.h"

bool Shader::Load(const char* vertShaderFile, const char* fragShaderFile)
{
    Filesystem* pFS    = Filesystem::Instance();
    File        vsFile = pFS->Read(vertShaderFile);
    File        fsFile = pFS->Read(fragShaderFile);

    GLuint vertShader;
    GLuint fragShader;

    if ( !CompileShader(vsFile, &vertShader, GL_VERTEX_SHADER) )
    {
        fprintf(stderr, "Failed to compile shader: '%s'\n", vertShaderFile);
        return false;
    }
    if ( !CompileShader(fsFile, &fragShader, GL_FRAGMENT_SHADER) )
    {
        fprintf(stderr, "Failed to compile shader: '%s'\n", fragShaderFile);
        return false;
    }

    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertShader);
    glAttachShader(m_Program, fragShader);
    glLinkProgram(m_Program);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return true;
}

void Shader::Use()
{
    glUseProgram(m_Program);
}

void Shader::Delete()
{
    glDeleteProgram(m_Program);
}
