#include "stdafx.h"
#include "Matrix4x4.h"
#include "ProjectionMatrix.h"
#include "ShaderProgram.h"
#include "TextureLoader.h"
#include <queue>
#include "opencv2/opencv.hpp"
#include <mutex>
#include <thread>

class DataProvider
{
protected:
    
    
public:
    virtual cv::Mat* getNextFrame() = 0;
    virtual double getFPS() = 0;
    virtual void restoreFPS() = 0;
    virtual void setFPS(double fps_in) = 0;
};
class VideoFile : public DataProvider
{
private:

    cv::VideoCapture mCap;
    bool mCacheRunning;
    cv::Mat* mCache;
    unsigned int mCacheSize;
    double mFPS;

    unsigned int mCurrentFrame;
    unsigned int mCachedFrames;
    unsigned int mCacheIndex;
    bool mCachingRunning;

    std::mutex modifyingCache;

    std::thread t;

    bool getImageFromCap(cv::Mat& frame_in)
    {
        if (!mCap.grab())
        {
            std::cout << "could not grab";
            return false;
        }
        mCap.retrieve(frame_in, CV_CAP_OPENNI_BGR_IMAGE);
        if (frame_in.empty())
        {
            return false;
        }
        return true;
    }

    void cache()
    {
        while (mCachedFrames < (mCacheSize -2))
        {
            if (!getImageFromCap(mCache[mCacheIndex]))
            {
                mCachingRunning = false;
                return;
            }
            {
                std::lock_guard<std::mutex> guard(modifyingCache);
                mCacheIndex++;
                mCacheIndex = mCacheIndex % mCacheSize;
                mCachedFrames++;
            }
        }
        mCachingRunning = false;
    }

public:
    VideoFile(const std::string& path_in, unsigned int cacheSize_in)    : mCacheRunning(true)
                                                                        , mCache(new cv::Mat[cacheSize_in])
                                                                        , mCacheSize(cacheSize_in)
                                                                        , mFPS(0)
                                                                        , mCurrentFrame(0)
                                                                        , mCachedFrames(0)
                                                                        , mCacheIndex(0)
                                                                        , modifyingCache()
                                                                        , mCachingRunning(true)
    {

        mCap.open(path_in);
        mFPS = mCap.get(CV_CAP_PROP_FPS);
        if (mCap.isOpened())
        {
            t = std::thread(&VideoFile::cache, this);
            t.detach();
        }
        else {
            std::cout << "error";
        }
    }

    VideoFile() :                 mCacheRunning(true)
                                , mCache(new cv::Mat[3])
                                , mCacheSize(3)
                                , mFPS(0)
                                , mCurrentFrame(0)
                                , mCachedFrames(0)
                                , mCacheIndex(0)
                                , modifyingCache()
                                , mCachingRunning(true)
    {

        mCap.open(0);
        mFPS = mCap.get(CV_CAP_PROP_FPS);
        if (mCap.isOpened())
        {
            t = std::thread(&VideoFile::cache, this);
            t.detach();
        }
        else {
            std::cout << "error";
        }
    }

    virtual void restoreFPS()
    {
        mFPS = mCap.get(CV_CAP_PROP_FPS);
    }
    virtual void setFPS(double fps_in)
    {
        mFPS = fps_in;
    }

    ~VideoFile()
    {
        delete[] mCache;
    }

    virtual cv::Mat* getNextFrame()
    {
        if (mCachedFrames == 0)
        {
            return nullptr;
        }

        cv::Mat* result = mCache + mCurrentFrame;
        std::lock_guard<std::mutex> guard(modifyingCache);
        mCachedFrames--;
        mCurrentFrame++;
        mCurrentFrame = mCurrentFrame % mCacheSize;
        if (mCachedFrames < (mCacheSize / 2) && !mCachingRunning)
        {
            mCachingRunning = true;
            std::thread t = std::thread(&VideoFile::cache, this);
            t.detach();
        }
        else {
            std::cout << "error";
        }
        return result;

    }
    virtual double getFPS()
    {
        return mFPS;
    }
};



std::string fragmentShader("shaders\\habrFragmentShader.fs");
std::string vertexShader("shaders\\habrVertexShader.vs");

std::string waveFragmentShader("shaders\\habrWaveFragmentShader.fs");
std::string waveVertexShader("shaders\\habrWaveVertexShader.vs");

std::string videoPath("b.mp4");
bool paused = false;
float movement = 2;
double flagSpeed = 0.01;
Matrix4x4 WVOProjection;

int windowWidth = 1900;
int windowHeight = 1000;
int linesInFlag = 30;
GLuint mVBO;        //vertices to draw
GLuint mIBO;        //indecis to draw

DataProvider* ptCap = nullptr;


ShaderProgramm *ptShaderProg;
ShaderProgramm *ptSimpleShaderProg;
ShaderProgramm *ptWaveShaderProg;
TextureLoader  *pt2DTexture;
bool wave = true;

/*void getCameraImage(cv::VideoCapture &cap, cv::Mat &frame)
{
    if (!cap.grab())
    {
        std::cout << "could not grab";
        return;
    }

    cap.retrieve(frame, CV_CAP_OPENNI_BGR_IMAGE);
    if (!frame.empty())
    {
        //cv::cvtColor(frame, frame, CV_BGR2RGBA);
        //cv::Point2f pt(frame.cols / 2.0, frame.rows / 2.0);
        //cv::Mat r = getRotationMatrix2D(pt, 180, 1.0);
        //warpAffine(frame, frame, r, cv::Size(frame.cols, frame.rows));
        //cv::flip(frame, frame, 1);
    }
}*/

void KeyboardInput(unsigned char key_in, int x_in, int y_in)
{    
    static bool fpsSwitch = true;
    switch (key_in)
    {
    case 'f':
    case 'F':
        
        if (fpsSwitch)
        {
            ptCap->setFPS(0);
        }
        else
        {
            ptCap->restoreFPS();
        }
        fpsSwitch = !fpsSwitch;
        break;
    case 'S':
        flagSpeed += 0.002;
        break;
    case 's':
        //static bool useCamera = true;
        //useCamera = !useCamera;
        flagSpeed -= 0.002;
        break;
    case 'p':
    case 'P':
        paused = !paused;
        break;
    case 'w':
    case 'W':
        wave = !wave;
        if (wave)
        {
            ptShaderProg = ptWaveShaderProg;
        }
        else
        {
            ptShaderProg = ptSimpleShaderProg;
        }
        
        break;

    }
    glutPostRedisplay();
}

void genFlag(OglVertexType width_in, OglVertexType height_in, int partsNum_in, std::vector<OglVertexType> &vertices_out, std::vector<GLuint> &indecis_out)
{

    OglVertexType xHalf = width_in / 2;
    OglVertexType yHalf = height_in / 2;

    OglVertexType coordOffset = width_in / partsNum_in;
    OglVertexType texOffset = 1.0 / partsNum_in;

    for (int i = 1; i < (partsNum_in + 1); i++)
    {
        /*vertices_out.push_back(-xHalf + coordOffset * (i - 1)); vertices_out.push_back(-yHalf); vertices_out.push_back(0); vertices_out.push_back(texOffset*(i - 1)); vertices_out.push_back(0);
        vertices_out.push_back(-xHalf + coordOffset * (i - 1)); vertices_out.push_back(yHalf); vertices_out.push_back(0); vertices_out.push_back(texOffset*(i - 1)); vertices_out.push_back(1);
        vertices_out.push_back(-xHalf + coordOffset * i);       vertices_out.push_back(yHalf); vertices_out.push_back(0); vertices_out.push_back(texOffset*(i));     vertices_out.push_back(1);
        vertices_out.push_back(-xHalf + coordOffset * i);       vertices_out.push_back(-yHalf); vertices_out.push_back(0); vertices_out.push_back(texOffset*(i));     vertices_out.push_back(0);*/
        vertices_out.push_back(-xHalf + coordOffset * (i - 1)); vertices_out.push_back(-yHalf); vertices_out.push_back(0); vertices_out.push_back(texOffset*(i - 1)); vertices_out.push_back(1);
        vertices_out.push_back(-xHalf + coordOffset * (i - 1)); vertices_out.push_back(yHalf); vertices_out.push_back(0); vertices_out.push_back(texOffset*(i - 1)); vertices_out.push_back(0);
        vertices_out.push_back(-xHalf + coordOffset * i);       vertices_out.push_back(yHalf); vertices_out.push_back(0); vertices_out.push_back(texOffset*(i));     vertices_out.push_back(0);
        vertices_out.push_back(-xHalf + coordOffset * i);       vertices_out.push_back(-yHalf); vertices_out.push_back(0); vertices_out.push_back(texOffset*(i));     vertices_out.push_back(1);
        if (i != 0)
        {
            unsigned int vertNum = i * 4;

            indecis_out.push_back(vertNum - 4); indecis_out.push_back(vertNum - 3); indecis_out.push_back(vertNum - 2);
            indecis_out.push_back(vertNum - 2); indecis_out.push_back(vertNum - 1); indecis_out.push_back(vertNum - 4);
        }
    }
}
std::chrono::time_point<std::chrono::system_clock> frame_StartedRendering = std::chrono::system_clock::now();
std::chrono::time_point<std::chrono::system_clock> flag_StartedRendering = std::chrono::system_clock::now();
void IddleFunc(void)
{
    if (paused)
    {
        glutPostRedisplay();
        return;
    }

    std::chrono::time_point<std::chrono::system_clock> today = std::chrono::system_clock::now();

    if (wave)
    {

        std::chrono::duration<double> flag_elapsed_seconds = today - flag_StartedRendering;
        if (flag_elapsed_seconds.count() >= flagSpeed)
        {
            flag_StartedRendering = std::chrono::system_clock::now();
            if (movement >= 360)
            {
                movement = 0;
            }
            movement += 1;
        }
    }
    bool getNextFrameFlag = true;
    if (ptCap->getFPS() != 0)
    {
        std::chrono::duration<double> last_frame_elapsed_seconds = today - frame_StartedRendering;
        if (last_frame_elapsed_seconds.count() < 0.04)
        {
            std::cout << "test";
        }
        if (last_frame_elapsed_seconds.count() >= (1.0 / (ptCap->getFPS())))
        {
            frame_StartedRendering = std::chrono::system_clock::now();
            getNextFrameFlag = true;
        }
        else
        {
            getNextFrameFlag = false;
        }
    }

    if (getNextFrameFlag)
    {
        cv::Mat *frame = ptCap->getNextFrame();
        if (frame != nullptr)
        {
            pt2DTexture->RewriteTexture(frame->ptr(), GL_TEXTURE0, GL_BGR);
        }
    }

    glutPostRedisplay();
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(ptShaderProg->getId());
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OglVertexType)*5, (const GLvoid*)(sizeof(OglVertexType)* 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(OglVertexType)* 5, (const GLvoid*)(sizeof(OglVertexType)* 3));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);

    GLuint id = ptShaderProg->getAttributeId("ProjectionViewMatrix");
    glUniformMatrix4fv(id, 1, GL_TRUE, &WVOProjection.mMatrix[0][0]);

    if (wave)
    {
        GLuint params = ptShaderProg->getAttributeId("params");
        glUniform4f(params, 1, 14, -0.5, movement);
    }
    

    GLuint TextureSamplerId = ptShaderProg->getAttributeId("mTexel");
    pt2DTexture->Bind(GL_TEXTURE0);
    glUniform1i(TextureSamplerId, 0);

    glDrawElements(GL_TRIANGLES, (linesInFlag * 2) * 3, GL_UNSIGNED_INT, ((void*)(0 * sizeof(GLuint))));

    glutSwapBuffers();
}

void createVertexData()
{
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mIBO);
    std::vector<OglVertexType> vertices;
    std::vector<GLuint> indecis;

    genFlag(3840.0 / 2160.0, 1, linesInFlag, vertices, indecis);

    if (!vertices.empty())
    {
        size_t vertSize = vertices.size() * sizeof(OglVertexType);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, vertSize, vertices.data(), GL_STATIC_DRAW);
    }

    if (!indecis.empty())
    {
        size_t indecisSize = indecis.size() * sizeof(GLuint);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indecisSize, indecis.data(), GL_STATIC_DRAW);
    }
}

std::string getGLSLversionFromVersionString(const GLubyte* fromGL_in)
{
    if (fromGL_in == nullptr)
    {
        return "";
    }
    if (fromGL_in[0] == '\0')
    {
        return "";
    }

    std::string result;
    int counter = 0;

    for (int i = 0; ((fromGL_in[i] != '\0') && (fromGL_in[i] != ' ')); i++)
    {
        if ((fromGL_in[i] >= '0') && (fromGL_in[i] <= '9')) { result += fromGL_in[i]; continue; }
        if (fromGL_in[i] == '.')
        {
            if (counter == 1) { break; }
            counter++;
            continue;
        }
        return "";
    }

    return result;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc_ = 1;
    char* argv_ = const_cast<char*>("BuggiFly");
    Matrix4x4 objectMatr;
    objectMatr[2][3] = 5;

    ProjectionMatrix projMatr(1, 100, windowWidth, windowHeight, 30.0f);
    WVOProjection = projMatr * objectMatr;
    VideoFile vdFile (videoPath, 40);
    ptCap = &vdFile;

    //window creation
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutInit(&argc_, &argv_);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutCreateWindow("HabraProj");

    glewInit();

    //glut callbacks
    glutIdleFunc(IddleFunc);
    glutDisplayFunc(renderScene);
    glutKeyboardFunc(KeyboardInput);

    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    ShaderProgramm ShaderProg;
    ShaderProgramm WavedShaderProg;
    const GLubyte* shadingVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::string glslVersion = getGLSLversionFromVersionString(shadingVersion);

    ShaderProg.AddShader(vertexShader, glslVersion, GL_VERTEX_SHADER);
    ShaderProg.AddShader(fragmentShader, glslVersion, GL_FRAGMENT_SHADER);
    ShaderProg.LinkProgramm();
    ShaderProg.initAttribute("ProjectionViewMatrix");
    ShaderProg.initAttribute("mTexel");

    WavedShaderProg.AddShader(waveVertexShader, glslVersion, GL_VERTEX_SHADER);
    WavedShaderProg.AddShader(waveFragmentShader, glslVersion, GL_FRAGMENT_SHADER);
    WavedShaderProg.LinkProgramm();
    WavedShaderProg.initAttribute("ProjectionViewMatrix");
    WavedShaderProg.initAttribute("params");
    WavedShaderProg.initAttribute("mTexel");

    ptShaderProg = &WavedShaderProg;

    ptSimpleShaderProg = &ShaderProg;
    ptWaveShaderProg = &WavedShaderProg;

    createVertexData();

    TextureLoader Texture(GL_TEXTURE_2D, 1);
    pt2DTexture = &Texture;
    cv::Mat* firstFrame = nullptr;
    do
    {
        firstFrame = vdFile.getNextFrame();
    } while (firstFrame == nullptr);
    //getCameraImage(cap, frame);
    if (firstFrame->empty())
    {
        return -1;
    }
    Texture.getTextureVars();
    GLuint gSampler = ShaderProg.getAttributeId("mTexel");
    Texture.Load((*firstFrame).ptr(), "image", (*firstFrame).cols, (*firstFrame).rows, GL_BGR);
    glUniform1i(gSampler, 0);
    //glutFullScreenToggle();
    glutMainLoop();
}