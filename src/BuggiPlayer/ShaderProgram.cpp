#include "stdafx.h"

#include "ShaderProgram.h"
#include "FileReader.h"

ShaderProgramm::ShaderProgramm() : mProgramId(glCreateProgram())
{

}

bool ShaderProgramm::AddShader(const std::string& path_in, const std::string& versionToAppend_in, GLenum shaderType_in)
{
    if (path_in.empty())
    {
        return false;
    }
    FileReader fileReader;
    std::string shaderText;
    fileReader.readAll(path_in, shaderText);
    if (shaderText.empty())
    {
        return false;
    }

    if (!versionToAppend_in.empty())
    {
        shaderText = std::string("#version ") + versionToAppend_in + std::string("\n\n") + shaderText;
    }

    AddShader(shaderText, shaderType_in);

    return true;
}

void ShaderProgramm::setName(const std::string& name_in)
{
    mName = name_in;
}

const std::string& ShaderProgramm::getName() const
{
    return mName;
}

bool ShaderProgramm::initAttribute(std::string name_in)
{
    //TODO: add chech, if programm existiong and correct, and has been build

    GLuint varId = glGetUniformLocation(mProgramId, name_in.c_str());
    if (varId == -1)
    {
        return false;
    }

    mAttributesIDs[name_in] = varId;

    return true;
}
GLuint ShaderProgramm::getAttributeId(std::string name_in)
{
    /*TODO: this will create attribute if not exist, rewrite to get attributes correct
    */
    return mAttributesIDs[name_in];
}

std::string ShaderProgramm::shaderTypeToString(GLenum shaderType_in)
{
    switch (shaderType_in)
    {
        case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
        default: return "UNKNOWN";
    }
}

bool ShaderProgramm::AddShader(std::string & shaderText_in, GLenum shaderType_in)
{
    if (mProgramId == 0)
    {
        std::cout << "ERROR: ShaderProgramm::AddShader shader programm does not exist, do nothing";
        return false;
    }
    if (shaderText_in.empty()) {
        std::cout << "ShaderProgramm::AddShader empty input string"<<std::endl;
        return false;
    }
    GLuint ShaderObj = glCreateShader(shaderType_in);

    if (ShaderObj == 0) {
        std::cout << "ShaderProgramm::AddShader can not create shader programm";
        return false;
    }

    const GLchar* p[1];
    p[0] = shaderText_in.c_str();
    GLint Lengths[1];
    Lengths[0] = shaderText_in.size();
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        std::cout << "Error compiling shader type" << shaderTypeToString(shaderType_in) << InfoLog;
        glDeleteShader(ShaderObj);
        return false;
    }

    glAttachShader(mProgramId, ShaderObj);
    //glLinkProgram(mProgramId);
    return true;
}

bool ShaderProgramm::LinkProgramm()
{
    /*
    TODO:
    add ore validations, and additional checks
    */
    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(mProgramId);
    glGetProgramiv(mProgramId, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(mProgramId, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        glDeleteProgram(mProgramId);
        mProgramId = 0;
        return false;
    }

    glValidateProgram(mProgramId);
    glGetProgramiv(mProgramId, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(mProgramId, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        glDeleteProgram(mProgramId);
        mProgramId = 0;
        return false;
    }

    glUseProgram(mProgramId);
    return true;
}
