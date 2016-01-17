#pragma once

class Matrix4x4
{
public:
    OglVertexType mMatrix[4][4];

    Matrix4x4();
    Matrix4x4& operator = (const Matrix4x4& other_in);
    Matrix4x4(const Matrix4x4& other_in);

    OglVertexType* operator[](int index_in);
    Matrix4x4 operator*(Matrix4x4& marix_in);

    std::string toString();
    void toIdentity();
};
