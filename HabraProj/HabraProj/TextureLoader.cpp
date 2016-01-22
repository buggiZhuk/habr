#include "stdafx.h"

#include "TextureLoader.h"

TexData TexData::EMPTY;

TextureLoader::TextureLoader(GLenum textureTarget_in, GLenum textureUnit_in) : mTextureData()
                                                                                , mTextureTarget(textureTarget_in)
                                                                                , mTexture(0)
                                                                                , mTextureUnit(textureUnit_in)
{   
}

TextureLoader::~TextureLoader()
{
    glDeleteTextures(1, &mTexture);
}
bool TextureLoader::RewriteTexture(GLubyte* texData_in, GLenum textureUnit_in, GLenum format)
{
    glBindTexture(textureUnit_in, mTexture);
    glTexImage2D(mTextureTarget, 0, GL_RGBA, mTextureData.mRealWidth, mTextureData.mRealHeight, 0, format, GL_UNSIGNED_BYTE, (void*)texData_in);

    return true;
}

//exData.mRealWidth, texData.mRealHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)textura
bool TextureLoader::Load(GLubyte* texData_in, std::string name_in, int width_in, int height_in, GLenum format)
{
    mTextureData.mRealWidth = width_in;
    mTextureData.mRealHeight = height_in;
    mTextureData.mName = name_in;

    GLubyte* textura = texData_in;

    glBindTexture(mTextureTarget, mTexture);
    if (glGetError() != GL_NO_ERROR)
    {
        std::cout << "can not bind texture";
        return false;
    }

    glTexImage2D(mTextureTarget, 0, GL_RGBA, mTextureData.mRealWidth, mTextureData.mRealHeight, 0, format, GL_UNSIGNED_BYTE, (void*)textura);

    glTexParameterf(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /*
    TODO:
    refactor: filing texData should be in one place
    */
    mTextureData.mTextureUnit = mTextureUnit;

    return true;
}

bool TextureLoader::getTextureVars()
{
    glGenTextures(1, &mTexture);

    glBindTexture(mTextureTarget, mTexture);

    if (glGetError() != GL_NO_ERROR)
    {
        std::cout << "can not generate texture";
        return false;
    }

    return true;
}

GLenum TextureLoader::TexUnitFromInt(int num_in)
{
    switch (num_in)
    {
    case 0:  return GL_TEXTURE0;
    case 1:  return GL_TEXTURE1;
    case 2:  return GL_TEXTURE2;
    case 3:  return GL_TEXTURE3;
    case 4:  return GL_TEXTURE4;
    case 5:  return GL_TEXTURE5;
    case 6:  return GL_TEXTURE6;
    case 7:  return GL_TEXTURE7;
    case 8:  return GL_TEXTURE8;
    case 9:  return GL_TEXTURE9;
    case 10: return GL_TEXTURE10;
    case 11: return GL_TEXTURE11;
    case 12: return GL_TEXTURE12;
    case 13: return GL_TEXTURE13;
    case 14: return GL_TEXTURE14;
    case 15: return GL_TEXTURE15;
    case 16: return GL_TEXTURE16;
    case 17: return GL_TEXTURE17;
    case 18: return GL_TEXTURE18;
    case 19: return GL_TEXTURE19;
    case 20: return GL_TEXTURE20;
    }

    return GL_NONE;
}

bool TextureLoader::Bind()
{
    int textureToUse = -1;
    
    if (textureToUse == -1)
    {
        std::cout << "ERROR: could not select texture to bind";
        return false;
    }

    glActiveTexture(mTextureUnit);
    glBindTexture(mTextureTarget, mTexture);

    return true;
}

/*TextureLoader::TextureLoader()  : mTextureData()
                                , mTextureTarget(GL_NONE)
                                , mMaxTextures(0)
                                , mCurrentTexture(0)
                                , mTextures(nullptr)
{

}*/
