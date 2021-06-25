#include "Vector3.h"
#include "Quaternion.h"
#include "Constants.h"

namespace Stulu {


    Vector3::Vector3() :x(0.0), y(0.0), z(0.0) {};

    Vector3::Vector3(float uX, float uY, float uZ) :x(uX), y(uY), z(uZ) {}

    Vector3::~Vector3() {}



    Vector3::Vector3(const Vector3& v) :x(v.x), y(v.y), z(v.z) {}

    Vector3& Vector3::operator=(const Vector3& v) {

        x = v.x;
        y = v.y;
        z = v.z;

        return *this;

    }


    void Vector3::operator+=(const Vector3& v) {

        x += v.x;
        y += v.y;
        z += v.z;

    }

    Vector3 Vector3::operator+(const Vector3& v)const {


        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    void Vector3::operator-=(const Vector3& v) {

        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    Vector3 Vector3::operator-(const Vector3& v)const {

        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    void Vector3::operator*=(const float s) {

        x *= s;
        y *= s;
        z *= s;

    }

    Vector3 Vector3::operator*(const float s) const {

        return Vector3(s * x, s * y, s * z);
    }


    void Vector3::operator /=(const float s) {

        x = x / s;
        y = y / s;
        z = z / s;
    }

    Vector3 Vector3::operator/(const float s)const {

        return Vector3(x / s, y / s, z / s);
    }

    float Vector3::operator*(const Vector3& v) const {

        return x * v.x + y * v.y + z * v.z;

    }

    float Vector3::dot(const Vector3& v) const {

        return x * v.x + y * v.y + z * v.z;

    }


    float Vector3::angle(const Vector3& v) {

        float theta;

        Vector3 u = v;
        Vector3 m = *this;

        theta = dot(u) / (m.magnitude() * u.magnitude());

        theta = RadToDegrees(acos(theta));

        return theta;

    }

    Vector3 Vector3::cross(const Vector3& v) const {

        return Vector3(y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);

    }

    void Vector3::operator %=(const Vector3& v) {

        *this = cross(v);

    }

    Vector3 Vector3::operator %(const Vector3& v) const {

        return Vector3(y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);
    }

    void Vector3::normalize() {

        float mag = std::sqrt(x * x + y * y + z * z);

        if (mag > 0.0f) {

            float oneOverMag = 1.0f / mag;

            x = x * oneOverMag;
            y = y * oneOverMag;
            z = z * oneOverMag;

        }

    }

    void Vector3::conjugate() {

        x = -1 * x;
        y = -1 * y;
        z = -1 * z;

    }


    float Vector3::magnitude() {

        float magnitude = std::sqrt(x * x + y * y + z * z);

        return magnitude;

    }

    float Vector3::magnitudeSquare() {

        float magnitude = x * x + y * y + z * z;

        return magnitude;
    }


    void Vector3::zero() {
        x = 0;
        y = 0;
        z = 0;
    }

    void Vector3::absolute() {

        x = std::abs(x);
        y = std::abs(y);
        z = std::abs(z);

    }


    Vector3 Vector3::rotateVectorAboutAngleAndAxis(float uAngle, Vector3& uAxis) {

        Quaternion p(0, (*this));

        uAxis.normalize();

        Quaternion q(uAngle, uAxis);

        q.convertToUnitNormQuaternion();

        Quaternion qInverse = q.inverse();

        Quaternion rotatedVector = q * p * qInverse;

        return rotatedVector.v;

    }


    void Vector3::show() {

        std::cout << "(" << x << "," << y << "," << z << ")" << std::endl;
    }


    void Vector3::negate() {

        x = -1 * x;
        y = -1 * y;
        z = -1 * z;
    }

}