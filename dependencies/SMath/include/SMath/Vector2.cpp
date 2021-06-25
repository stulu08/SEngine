#include "Vector2.h"
#include "Quaternion.h"
#include "Constants.h"

namespace Stulu {


    Vector2::Vector2() :x(0.0), y(0.0) {};

    Vector2::Vector2(float uX, float uY) :x(uX), y(uY) {}

    Vector2::~Vector2() {}



    Vector2::Vector2(const Vector2& v) :x(v.x), y(v.y) {}

    Vector2& Vector2::operator=(const Vector2& v) {

        x = v.x;
        y = v.y;

        return *this;

    }


    void Vector2::operator+=(const Vector2& v) {

        x += v.x;
        y += v.y;

    }

    Vector2 Vector2::operator+(const Vector2& v)const {


        return Vector2(x + v.x, y + v.y);
    }

    void Vector2::operator-=(const Vector2& v) {

        x -= v.x;
        y -= v.y;
    }

    Vector2 Vector2::operator-(const Vector2& v)const {

        return Vector2(x - v.x, y - v.y);
    }

    void Vector2::operator*=(const float s) {

        x *= s;
        y *= s;

    }

    Vector2 Vector2::operator*(const float s) const {

        return Vector2(s * x, s * y);
    }


    void Vector2::operator /=(const float s) {

        x = x / s;
        y = y / s;
    }

    Vector2 Vector2::operator/(const float s)const {

        return Vector2(x / s, y / s);
    }

    float Vector2::operator*(const Vector2& v) const {

        return x * v.x + y * v.y;

    }

    float Vector2::dot(const Vector2& v) const {

        return x * v.x + y * v.y;

    }

    float Vector2::angle(const Vector2& v) {

        float theta;

        Vector2 u = v;
        Vector2 m = *this;

        theta = dot(u) / (m.magnitude() * u.magnitude());

        theta = RadToDegrees(acos(theta));

        return theta;

    }

    /*Vector3 Vector2::cross(const Vector2& v) const {

        return Vector3(y * 0 - 0 * v.y,
            0 * v.x - x * 0,
            x * v.y - y * v.x);

    }

    void Vector2::operator %=(const Vector2& v) {

        *this = cross(v);

    }

    Vector2 Vector2::operator %(const Vector2& v) const {

        return Vector2(y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);
    }*/

    void Vector2::normalize() {

        float mag = std::sqrt(x * x + y * y);

        if (mag > 0.0f) {

            float oneOverMag = 1.0f / mag;

            x = x * oneOverMag;
            y = y * oneOverMag;
        }

    }

    void Vector2::conjugate() {

        x = -1 * x;
        y = -1 * y;

    }


    float Vector2::magnitude() {

        float magnitude = std::sqrt(x * x + y * y);

        return magnitude;

    }

    float Vector2::magnitudeSquare() {

        float magnitude = x * x + y * y;

        return magnitude;
    }


    void Vector2::zero() {
        x = 0;
        y = 0;
    }

    void Vector2::absolute() {

        x = std::abs(x);
        y = std::abs(y);

    }


    void Vector2::show() {

        std::cout << "(" << x << "," << y << ")" << std::endl;
    }


    void Vector2::negate() {

        x = -1 * x;
        y = -1 * y;
    }

}