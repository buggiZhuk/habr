#pragma once

class TexData
{
public:
    TexData()   : mName()
                , mRealWidth(0)
                , mRealHeight(0)
                , mTextureUnit(GL_NONE)
    {
    }
    std::string     mName;
    unsigned int    mRealWidth;
    unsigned int    mRealHeight;
    GLenum          mTextureUnit;
    bool operator==(const TexData& texData_in)
    {

    }

    static TexData EMPTY;
};

class TextureLoader
{
    GLenum TexUnitFromInt(int num_in);

    std::vector<TexData>    mTextureData;    
    const GLenum            mTextureTarget;
    GLuint                  mMaxTextures;
    unsigned int            mCurrentTexture;
    GLuint*                 mTextures;

public:
    TextureLoader();
    TextureLoader(GLenum textureTarget_in, GLuint maxTextureNum_in);
    ~TextureLoader();

    TexData& operator[](GLenum textureUnit_in);
    TexData& operator[](std::string& name_in);

    bool getTextureVars();

    bool Load(GLubyte* texData_in, std::string name_in, int width_in, int height_in, GLenum format);
    bool RewriteTexture(GLubyte* texData_in, GLenum textureUnit_in, GLenum format);
    bool Bind(GLenum textureUnit_in);
};