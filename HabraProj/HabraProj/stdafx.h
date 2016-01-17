#pragma once

#define GLEW_STATIC
#define FREEGLUT_STATIC

#include <Windows.h>
#include "opencv2/opencv.hpp"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <chrono>
#include <fstream>
#include <string>


typedef GLfloat OglVertexType;
#define M_PI       3.14159265358979323846
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))