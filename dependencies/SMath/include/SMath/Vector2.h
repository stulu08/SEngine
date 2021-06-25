#pragma once
#ifdef SMath_Vector2_EXPORTS
#define SMath_Vector2 __declspec(dllexport)
#else
#define SMath_Vector2 __declspec(dllimport)
#endif

#include <stdio.h>
#include <cmath>
#include <iostream>
namespace Stulu {

    class Vector2 {

    private:

    public:

        float x;
        float y;


        SMath_Vector2 Vector2();


        SMath_Vector2 Vector2(float uX, float uY);

        SMath_Vector2 ~Vector2();


        SMath_Vector2 Vector2(const Vector2& v);

        SMath_Vector2 Vector2& operator=(const Vector2& v);



        SMath_Vector2 void operator+=(const Vector2& v);

        SMath_Vector2 Vector2 operator+(const Vector2& v)const;


        SMath_Vector2 void operator-=(const Vector2& v);

        SMath_Vector2 Vector2 operator-(const Vector2& v)const;


        SMath_Vector2 void operator*=(const float s);

        SMath_Vector2 Vector2 operator*(const float s)const;



        SMath_Vector2 void operator /=(const float s);

        SMath_Vector2 Vector2 operator/(const float s)const;

        SMath_Vector2 float operator*(const Vector2& v) const;

        SMath_Vector2 float dot(const Vector2& v) const;

        SMath_Vector2 float angle(const Vector2& v);


        //SMath_Vector2 void operator %=(const Vector2& v);

        //SMath_Vector2 Vector2 operator %(const Vector2& v) const;

        //SMath_Vector2 Vector3 cross(const Vector2& v) const;

        SMath_Vector2 void conjugate();

        SMath_Vector2 void normalize();

        SMath_Vector2 float magnitude();

        SMath_Vector2 float magnitudeSquare();


        SMath_Vector2 void zero();

        SMath_Vector2 void absolute();

        //SMath_Vector2 Vector2 rotateVectorAboutAngleAndAxis(float uAngle, Vector2& uAxis);

        SMath_Vector2 void show();

        SMath_Vector2 void negate();


    };

}

