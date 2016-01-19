#include "stdafx.h"
#include "Matrix4x4.h"
#include "ProjectionMatrix.h"
#include "ShaderProgram.h"
#include "TextureLoader.h"
#include <queue>
#include "opencv2/opencv.hpp"
#include <mutex>
#include <thread>


std::string fragmentShader("shaders\\habrFragmentShader.fs");
std::string vertexShader("shaders\\habrVertexShader.vs");
std::string videoPath("a.flv");

class DataProvider
{
protected:
    cv::VideoCapture mCap;
    bool getImageFromCap(cv::Mat& frame_in)
    {
        if (!mCap.grab())
        {
            std::cout << "could not grab";
            return false;
        }
        mCap.retrieve(frame_in, CV_CAP_OPENNI_BGR_IMAGE);
    }
public:
    virtual cv::Mat* getNextFrame() = 0;
    virtual double getFPS() = 0;
};

class VideoFile : public DataProvider
{
    bool mCacheRunning;
    //int mCurrentImage;
    std::queue<cv::Mat> cache;
    unsigned int mCacheSize;
    double mFPS;
    bool mFrameAvailable;
    std::mutex settingAviability;
    std::mutex modifyingCache;

    void startCacheing()
    {
        size_t size = cache.size();
        do
        {
            std::lock_guard<std::mutex> guard(modifyingCache);
            cache.push(cv::Mat());
            getImageFromCap(cache.back());
            if (size >= 1)
            {
                setFrameAvailable(true);
            }
            size++;
        } while (size < mCacheSize);
        mCacheRunning = false;
    }

    bool getFrameAvailable()
    {
        std::lock_guard<std::mutex> guard(settingAviability);
        return mFrameAvailable;
    }
    void setFrameAvailable(bool available_in)
    {
        std::lock_guard<std::mutex> guard(settingAviability);
        mFrameAvailable = available_in;
    }
public:
    
    VideoFile(const std::string& path_in, unsigned int cacheSize_in) : mCacheRunning(true)
                                                                     , cache()
                                                                     , mCacheSize(cacheSize_in)
                                                                     , mFPS(0)
                                                                     , mFrameAvailable(false)
    {
        
        mCap.open(path_in);
        mFPS = mCap.get(CV_CAP_PROP_FPS);
        if (mCap.isOpened())
        {
            std::thread t = std::thread(&VideoFile::startCacheing, this);
            t.detach();
        }
        else {
            std::cout << "error";
        }
    }
    virtual cv::Mat* getNextFrame()
    {
        cv::Mat* result;
        if (getFrameAvailable())
        {
            result = &(cache.front());
        }
        else
        {
            result = nullptr;
        }
        return result;
    }
    void ReleaseLastFrame()
    {
        {
            std::lock_guard<std::mutex> guard(modifyingCache);
            cache.pop();
        }
        
        if (cache.size() == 0)
        {
            setFrameAvailable(false);
        }
        if ((mCacheSize / 2) > cache.size())
        {
            if (!mCacheRunning)
            {
                mCacheRunning = true;
                std::thread t = std::thread(&VideoFile::startCacheing, this);
                t.detach();
            }

        }
    }
    virtual double getFPS()
    {
        return mFPS;
    }

    void setFPS(double FPS_in)
    {
        mFPS = FPS_in;
    }

    void setRealFPS()
    {
        mFPS = mCap.get(CV_CAP_PROP_FPS);
    }
};

class CameraImageProvider : public DataProvider
{
public:
    CameraImageProvider()
    {
        mCap.open(0);
    }
    virtual cv::Mat* getNextFrame()
    {
        return nullptr;
    }
    virtual double getFPS()
    {
        return 0;
    }
};

enum class SOURCE
{
    CAMERA,
    VIDEO
};

int windowWidth = 1000;
int windowHeight = 1000;
int linesInFlag = 30;
GLuint mVBO;        //vertices to draw
GLuint mIBO;        //indecis to draw
VideoFile vdFile(videoPath, 100);


ShaderProgramm *ptShaderProg;
TextureLoader  *pt2DTexture;
cv::VideoCapture cap;

void getCameraImage(cv::VideoCapture &cap, cv::Mat &frame)
{
    if (!cap.grab())
    {
        std::cout << "could not grab";
        return;
    }
    //cv::Mat frame;
    cap.retrieve(frame, CV_CAP_OPENNI_BGR_IMAGE);
    //if (!frame.empty())
    {
        //cv::cvtColor(frame, frame, CV_BGR2RGBA);
        /*cv::Point2f pt(frame.cols / 2.0, frame.rows / 2.0);
        cv::Mat r = getRotationMatrix2D(pt, 180, 1.0);
        warpAffine(frame, frame, r, cv::Size(frame.cols, frame.rows));
        cv::flip(frame, frame, 1);*/
        //frames.push_back(frame);
    }
}

void KeyboardInput(unsigned char key_in, int x_in, int y_in)
{    
    switch (key_in)
    {
    case 'S':
    case 's':
        static bool useCamera = true;
        useCamera = !useCamera;
        cap.release();
        if (useCamera)
        {
            cap.open(0);
            
        }
        else
        {
            cap.open(videoPath);
        }

        if (!cap.isOpened())
        {
            //TODO: process errror
        }

        glutPostRedisplay();
        break;
    }
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
std::chrono::time_point<std::chrono::system_clock> startedRendering = std::chrono::system_clock::now();
void IddleFunc(void)
{
    std::chrono::time_point<std::chrono::system_clock> today = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = today - startedRendering;
    if (elapsed_seconds.count() >= (1.0 / vdFile.getFPS()))
    {
        cv::Mat *frame = vdFile.getNextFrame();
        if (frame != nullptr)
        {
            startedRendering = std::chrono::system_clock::now();
            pt2DTexture->RewriteTexture(frame->ptr(), GL_TEXTURE0, GL_BGR);
            glutPostRedisplay();
            vdFile.ReleaseLastFrame();
        }
    }
    
    
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ProjectionMatrix projMatr(1, 100, windowWidth, windowHeight, 30.0f);

    glUseProgram(ptShaderProg->getId());
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OglVertexType)*5, (const GLvoid*)(sizeof(OglVertexType)* 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(OglVertexType)* 5, (const GLvoid*)(sizeof(OglVertexType)* 3));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);

    Matrix4x4 objectMatr;
    objectMatr[2][3] = 5;
    Matrix4x4 WVOProjection = projMatr * objectMatr;
    GLuint id = ptShaderProg->getAttributeId("ProjectionViewMatrix");
    glUniformMatrix4fv(id, 1, GL_TRUE, &WVOProjection.mMatrix[0][0]);

    GLuint params = ptShaderProg->getAttributeId("param");
    static float movement = 2;
    if (movement >= 360)
    {
        movement = 0;
    }
    glUniform4f(params, 1, 14, -0.5, movement);
    movement += 2;

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
    genFlag(1, 1, linesInFlag, vertices, indecis);

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

    if (cap.open(videoPath))
    {
        std::cout << "camera is opened";
    }
    else {
        return -1;
    }

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
    const GLubyte* shadingVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::string glslVersion = getGLSLversionFromVersionString(shadingVersion);
    ShaderProg.AddShader(vertexShader, glslVersion, GL_VERTEX_SHADER);
    ShaderProg.AddShader(fragmentShader, glslVersion, GL_FRAGMENT_SHADER);
    ShaderProg.LinkProgramm();
    ShaderProg.initAttribute("ProjectionViewMatrix");
    ShaderProg.initAttribute("params");
    ptShaderProg= &ShaderProg;

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
    vdFile.ReleaseLastFrame();

    glutMainLoop();
}