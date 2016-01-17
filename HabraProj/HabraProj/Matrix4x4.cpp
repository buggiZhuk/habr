#include "stdafx.h"

#include "Matrix4x4.h"

OglVertexType* Matrix4x4::operator[](int index_in)
{
    /*
    TODO:
    add const variant of this function
    */
    return mMatrix[index_in];
}

Matrix4x4::Matrix4x4()
{
    for (int i = 0; i < 4; i++)
    for (int k = 0; k < 4; k++)
    {
        if (i == k)
        {
            mMatrix[i][k] = 1;
        } else {
            mMatrix[i][k] = 0;
        }
    }
}

Matrix4x4& Matrix4x4::operator = (const Matrix4x4& other_in)
{
    for (int i = 0; i < 4; i++)
    for (int k = 0; k < 4; k++)
    {
        mMatrix[i][k] = other_in.mMatrix[i][k];
    }

    return *this;
}

Matrix4x4::Matrix4x4(const Matrix4x4& other_in)
{
    for (int i = 0; i < 4; i++)
    for (int k = 0; k < 4; k++)
    {
        mMatrix[i][k] = other_in.mMatrix[i][k];
    }
}

Matrix4x4 Matrix4x4::operator*(Matrix4x4& marix_in)
{
    /*
    TODO:
    PASS const par into this function
    */
    Matrix4x4 result;
    for (int line = 0; line < 4; line++)
    for (int column = 0; column < 4; column++)
        result[line][column] = 0;

    for (int line = 0; line < 4; line++)
    for (int column = 0; column < 4; column++)
    {
        for (int i = 0; i < 4; i++)
            result[line][column] += mMatrix[line][i] * marix_in[i][column];
    }

    return result;
}

std::string Matrix4x4::toString()
{
    std::stringstream ss;

    for (int line = 0; line < 4; line++)
    for (int column = 0; column < 4; column++)
    {
        if (column != 0) { ss << ' '; }
        ss << mMatrix[line][column];
        if (column == 3){ ss << '\n'; }
    }

    std::string result = ss.str();
    return result;

}

void Matrix4x4::toIdentity()
{
    for (int i = 0; i < 4; i++)
    for (int k = 0; k < 4; k++)
    if (i != k)
    {
        mMatrix[i][k] = 0;
    }
    else
    {
        mMatrix[i][k] = 1;
    }
}
