#pragma once

#include <cmath>

struct Vector4 {
    float x;
    float y;
    float z;
    float w;

    inline Vector4() : x(0), y(0), z(0), w(0) {}

    inline Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    inline explicit Vector4(float value) : x(value), y(value), z(value), w(value) {}

    inline Vector4 operator+(const Vector4 &other) const {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    inline Vector4 operator+(const float other) const {
        return Vector4(x + other, y + other, z + other, w + other);
    }

    inline Vector4 operator-(const Vector4 &other) const {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    inline Vector4 operator-(const float other) const {
        return Vector4(x - other, y - other, z - other, w - other);
    }

    inline Vector4 operator*(const float value) const {
        return Vector4(x * value, y * value, z * value, w * value);
    }

    inline Vector4 operator*(const Vector4 &other) const {
        return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
    }

    inline Vector4 operator/(const float value) const {
        if (value != 0) {
            return Vector4(x / value, y / value, z / value, w / value);
        }
        return Vector4();
    }


    inline Vector4 operator-() const {
        return Vector4(-x, -y, -z, -w);
    }

    inline Vector4 &operator+=(const Vector4 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    inline Vector4 &operator-=(const Vector4 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    inline Vector4 &operator+=(const float value) {
        x += value;
        y += value;
        z += value;
        w += value;
        return *this;
    }

    inline Vector4 &operator-=(const float value) {
        x -= value;
        y -= value;
        z -= value;
        w -= value;
        return *this;
    }

    inline Vector4 &operator*=(const float value) {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
        return *this;
    }

    inline Vector4 &operator*=(const Vector4 &other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }

    inline Vector4 &operator/=(const float value) {
        x /= value;
        y /= value;
        z /= value;
        w /= value;
        return *this;
    }

    inline Vector4 &operator=(const Vector4 &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    inline bool operator==(const Vector4 &other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    inline bool operator!=(const Vector4 &other) const {
        return x != other.x || y != other.y || z != other.z || w != other.w;
    }

    inline float operator[](int index) const {
        return (&x)[index];
    }

    inline float &operator[](int index) {
        return (&x)[index];
    }

    inline void Zero() {
        x = y = z = w = 0;
    }

    inline bool NotHaveZero() {
        return x != 0 && y != 0 && z != 0 && w != 0;
    }

    inline float length() {
        return sqrt(x * x + y * y + z * z + w * w);
    }

};