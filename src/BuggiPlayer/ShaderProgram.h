#pragma once

class ShaderProgramm
{
private:
    std::string shaderTypeToString(GLenum shaderType_in);
    GLuint mProgramId;
    std::map<std::string, GLuint> mAttributesIDs;
    std::string mName;
public:
    ShaderProgramm();
    bool AddShader(std::string& shaderText_in, GLenum shaderType_in);
    bool AddShader(const std::string& shaderText_in, const std::string& versionToAppend_in, GLenum shaderType_in);
    bool LinkProgramm();
    void setName(const std::string& name_in);
    const std::string& getName() const;

    bool initAttribute(std::string name);
    GLuint getAttributeId(std::string name);

    GLuint getId()
    {
        return mProgramId;
    }
};
