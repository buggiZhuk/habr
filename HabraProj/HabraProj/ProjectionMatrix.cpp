#include "stdafx.h"

#include "ProjectionMatrix.h"

ProjectionMatrix::ProjectionMatrix() : mNear(1)
                                     , mFar(100)
                                     , mWidth(300)
                                     , mHeight(300)
                                     , mFOV(30.0f)
{
    recalcMatrix();
}

ProjectionMatrix::ProjectionMatrix(OglVertexType near_in, OglVertexType far_in, OglVertexType width_in, OglVertexType height_in, OglVertexType fov_in)  : mNear(near_in)
                                                                                                                                    , mFar(far_in)
                                                                                                                                    , mWidth(width_in)
                                                                                                                                    , mHeight(height_in)
                                                                                                                                    , mFOV(fov_in)
{
    recalcMatrix();
}

void ProjectionMatrix::recalcMatrix()
{
    const float ar = mWidth / mHeight;
    const float zRange = mNear - mFar;
    const float tanHalfFOV = tanf(ToRadian(mFOV / 2.0f));

    mMatrix[0][0] = 1.0f / (tanHalfFOV * ar); mMatrix[0][1] = 0.0f;              mMatrix[0][2] = 0.0f;            mMatrix[0][3] = 0.0;
    mMatrix[1][0] = 0.0f;                      mMatrix[1][1] = 1.0f / tanHalfFOV; mMatrix[1][2] = 0.0f;            mMatrix[1][3] = 0.0;
    mMatrix[2][0] = 0.0f;                     mMatrix[2][1] = 0.0f;              mMatrix[2][2] = (mNear - mFar) / zRange; mMatrix[2][3] = 2.0f*mFar*mNear / zRange;
    mMatrix[3][0] = 0.0f;                     mMatrix[3][1] = 0.0f;              mMatrix[3][2] = 1.0f;            mMatrix[3][3] = 0.0;
}

void ProjectionMatrix::setNear(OglVertexType near_in)
{
    mNear = near_in;
    recalcMatrix();
}

void ProjectionMatrix::setFar(OglVertexType far_in)
{
    mFar = far_in;
    recalcMatrix();
}

void ProjectionMatrix::setWidth(OglVertexType width_in)
{
    mWidth = width_in;
    recalcMatrix();
}

void ProjectionMatrix::setHeight(OglVertexType height_in)
{
    mHeight = height_in;
    recalcMatrix();
}

void ProjectionMatrix::setFOV(OglVertexType fov_in)
{
    mFOV = fov_in;
    recalcMatrix();
}
