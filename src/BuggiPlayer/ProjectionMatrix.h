#pragma once
#include "Matrix4x4.h"

class ProjectionMatrix : public Matrix4x4
{
protected:
    void recalcMatrix();
private:
    OglVertexType mNear;
    OglVertexType mFar;
    OglVertexType mWidth;
    OglVertexType mHeight;
    OglVertexType mFOV;
public:
    ProjectionMatrix();
    ProjectionMatrix(OglVertexType near_in, OglVertexType far_in, OglVertexType width_in, OglVertexType height_in, OglVertexType FOV_in);

    void setNear(OglVertexType near_in);
    void setFar(OglVertexType far_in);
    void setWidth(OglVertexType width_in);
    void setHeight(OglVertexType height_in);
    void setFOV(OglVertexType fov_in);
};