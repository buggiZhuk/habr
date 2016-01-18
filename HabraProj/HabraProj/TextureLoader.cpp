#include "stdafx.h"

#include "TextureLoader.h"

TexData TexData::EMPTY;

TextureLoader::TextureLoader(GLenum textureTarget_in, GLuint maxTextureNum_in)  : mTextureData()
                                                                                , mTextureTarget(textureTarget_in)
                                                                                , mMaxTextures(maxTextureNum_in)
                                                                                , mCurrentTexture(0)
                                                                                , mTextures(new GLuint[mMaxTextures])
{   
}

TextureLoader::~TextureLoader()
{
    glDeleteTextures(mMaxTextures, mTextures);
    delete[] mTextures;
}
bool TextureLoader::RewriteTexture(GLubyte* texData_in, GLenum textureUnit_in, GLenum format)
{
    glBindTexture(textureUnit_in, mTextures[mCurrentTexture - 1]);
    glTexImage2D(mTextureTarget, 0, GL_RGBA, mTextureData[0].mRealWidth, mTextureData[0].mRealHeight, 0, format, GL_UNSIGNED_BYTE, (void*)texData_in);
    GLenum currentTexUnit = TexUnitFromInt(mCurrentTexture);
    return true;
}

//exData.mRealWidth, texData.mRealHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)textura
bool TextureLoader::Load(GLubyte* texData_in, std::string name_in, int width_in, int height_in, GLenum format)
{
    TexData texData;
    texData.mRealWidth = width_in;
    texData.mRealHeight = height_in;
    texData.mName = name_in;

    GLubyte* textura = texData_in;

    glBindTexture(mTextureTarget, mTextures[mCurrentTexture]);
    if (glGetError() != GL_NO_ERROR)
    {
        std::cout << "can not bind texture";
        return false;
    }

    glTexImage2D(mTextureTarget, 0, GL_RGBA, texData.mRealWidth, texData.mRealHeight, 0, format, GL_UNSIGNED_BYTE, (void*)textura);
    //delete[] textura;
    glTexParameterf(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum currentTexUnit = TexUnitFromInt(mCurrentTexture);
    /*
    TODO:
    refactor: filing texData should be in one place
    */
    texData.mTextureUnit = currentTexUnit;
    mTextureData.push_back(texData);

    //uglu hack!!
    mCurrentTexture++;
    return true;
}
/*
bool TextureLoader::Load(boost::filesystem::path path_in, std::string name_in)
{

    if (mCurrentTexture == mMaxTextures)
    {
        buggi::general::InvalidArgumentException argException("Max Amount of textures has been already loaded");
        throw argException;
    }
    /*
    TODO:
    check name_in for unique
    */
    /*if (path_in.empty())
    {
        std::cout << "path is empty, do nothing";
        return false;
    }
    
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    fif = FreeImage_GetFileType((path_in.string().c_str()), 0);
    if (fif == FIF_UNKNOWN)
    {
        std::cout << "could not get type of image";
        return false;
        /*
        TODO: thow exception
        */
    /*}

    if (fif != FIF_PNG) {
        std::cout << "only PNG format is supported";
        return false;
    }
    
    FIBITMAP* bitmap = FreeImage_Load(fif, path_in.string().c_str());
    if (bitmap == NULL)
    {
        std::cout << "Could not parse image";
        return false;
        /*
        todo:
        throw exception;
        */
    /*}


    FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
    if (bitmap32 == NULL)
    {
        std::cout << "can not convert image to RGB!";
        return false;
        /*
        todo:
        throw exception;
        */
    /*}
    FreeImage_Unload(bitmap);
    TexData texData;
    texData.mRealWidth  = FreeImage_GetWidth(bitmap32);
    texData.mRealHeight = FreeImage_GetHeight(bitmap32);
    texData.mName       = name_in;

    std::cout << std::endl << "Image loaded and converted:" << std::endl \
        << "path:   " << path_in << std::endl \
        << "width:  " << texData.mRealWidth << std::endl \
        << "height: " << texData.mRealHeight << std::endl;

    /*
    TODO: remove this copypasted, dengereos, unknown code. I don't know how and why does it work;
    */
    /*BYTE* pixeles = FreeImage_GetBits(bitmap32);
    GLubyte* textura = new GLubyte[4 * texData.mRealWidth*texData.mRealHeight];
    for (int j = 0; j<texData.mRealWidth*texData.mRealHeight; j++){
        textura[j * 4 + 0] = pixeles[j * 4 + 2];
        textura[j * 4 + 1] = pixeles[j * 4 + 1];
        textura[j * 4 + 2] = pixeles[j * 4 + 0];
        textura[j * 4 + 3] = pixeles[j * 4 + 3];
        //cout<<j<<": "<<textura[j*4+0]<<"**"<<textura[j*4+1]<<"**"<<textura[j*4+2]<<"**"<<textura[j*4+3]<<endl;
    }
    FreeImage_Unload(bitmap32);
    return Load(textura, name_in, texData.mRealWidth, texData.mRealHeight, GL_RGBA);   
}
*/
TexData& TextureLoader::operator[](GLenum textureUnit_in)
{
    for (int i = 0; i < mTextureData.size(); i++)
    {
        if (mTextureData[i].mTextureUnit == textureUnit_in)
        {
            return mTextureData[i];
        }
    }

    return TexData::EMPTY;
}

TexData& TextureLoader::operator[](std::string& name_in)
{
    for (int i = 0; i < mTextureData.size(); i++)
    {
        if (mTextureData[i].mName == name_in)
        {
            return mTextureData[i];
        }
    }

    return TexData::EMPTY;
}

bool TextureLoader::getTextureVars()
{
    glGenTextures(mMaxTextures, mTextures);
    for (int i = 0; i < mMaxTextures; i++)
    {
        glBindTexture(mTextureTarget, mTextures[i]);
    }
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

bool TextureLoader::Bind(GLenum textureUnit_in)
{
    int textureToUse = -1;
    switch (textureUnit_in)
    {
    case GL_TEXTURE0: textureToUse = 0; break;
    case GL_TEXTURE1: textureToUse = 1; break;
    case GL_TEXTURE2: textureToUse = 2; break;
    default:
        std::cout << "Can not handle param!";
        return false;
    }

    if (textureToUse == -1)
    {
        std::cout << "ERROR: could not select texture to bind";
        return false;
    }
    if (textureToUse > mMaxTextures)
    {
        std::cout << "ERROR: TextureLoader::Bind does not have such bin texture number";
        return false;
    }

    glActiveTexture(textureUnit_in);
    glBindTexture(mTextureTarget, mTextures[textureToUse]);

    return true;
}

TextureLoader::TextureLoader()  : mTextureData()
                                , mTextureTarget(GL_NONE)
                                , mMaxTextures(0)
                                , mCurrentTexture(0)
                                , mTextures(nullptr)
{

}
