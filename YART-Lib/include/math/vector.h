#pragma once
#include "core/assert.h"
#include "core/yart.h"
#include <iostream>

namespace yart
{
    template <typename T>
    class Vector3
    {
    public:
        Vector3() : x(0), y(0), z(0)
        {
        }

        Vector3(T x, T y, T z) : x(x), y(y), z(z)
        {
            ASSERT(!HasNaNs());
        }

        Vector3(T arg) : x(arg), y(arg), z(arg)
        {
            ASSERT(!HasNaNs());
        }

        template <typename U>
        explicit Vector3(const Vector3<U>& vec) : x(vec.x), y(vec.y), z(vec.z)
        {
            ASSERT(!HasNaNs());
        }

        T& operator[](i32 i)
        {
            ASSERT(i >= 0 && i <= 2);
            if (i == 0)
                return x;
            if (i == 1)
                return y;
            return z;
        }

        T operator[](i32 i) const
        {
            return const_cast<Vector3<T>*>(this)->operator[](i);
        }

        Vector3<T> operator+(const Vector3<T>& other) const
        {
            ASSERT(!other.HasNaNs());
            return Vector3<T>(x + other.x, y + other.y, z + other.z);
        }

        Vector3<T>& operator+=(const Vector3<T>& other)
        {
            ASSERT(!other.HasNaNs());
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vector3<T> operator-(const Vector3<T>& other) const
        {
            ASSERT(!other.HasNaNs());
            return Vector3<T>(x - other.x, y - other.y, z - other.z);
        }

        Vector3<T>& operator-=(const Vector3<T>& other)
        {
            ASSERT(!other.HasNaNs());
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vector3<T> operator*(const Vector3<T>& other) const
        {
            ASSERT(!other.HasNaNs());
            return Vector3<T>(x * other.x, y * other.y, z * other.z);
        }

        Vector3<T>& operator*=(const Vector3<T>& other)
        {
            ASSERT(!other.HasNaNs());
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        Vector3<T> operator*(T s) const
        {
            ASSERT(!std::isnan(s));
            return Vector3<T>(x * s, y * s, z * s);
        }

        Vector3<T>& operator*=(T s)
        {
            ASSERT(!std::isnan(s));
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }

        Vector3<T> operator/(T s) const
        {
            ASSERT(s != 0);
            real inv = (real)1 / s;
            return *this * inv;
        }

        Vector3<T>& operator/=(T s)
        {
            ASSERT(s != 0);
            real inv = (real)1 / s;
            *this *= inv;
            return *this;
        }

        Vector3<T> operator-() const
        {
            return Vector3<T>(-x, -y, -z);
        }

        bool operator==(const Vector3<T>& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vector3<T>& other) const
        {
            return !(*this == other);
        }

        T MinComponent() const
        {
            return std::min(x, std::min(y, z));
        }

        T MaxComponent() const
        {
            return std::max(x, std::max(y, z));
        }

        // Returns the index of the component with the largest value
        i32 MaxDimension() const
        {
            return (x > y) ? ((x > z) ? 0 : 2) : ((y > z) ? 1 : 2);
        }

        bool HasNaNs() const
        {
            return std::isnan(x) || std::isnan(y) || std::isnan(z);
        }

    public:
        T x, y, z;
    };

    // Partial specialization for integer type
    template <>
    inline bool Vector3<i32>::HasNaNs() const
    {
        return false;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& out, const Vector3<T>& other)
    {
        out << "(" << other.x << ", " << other.y << ", " << other.z << ")";
        return out;
    }

    template <typename T>
    inline Vector3<T> operator*(T s, const Vector3<T>& v)
    {
        return v * s;
    }

    template <typename T>
    inline Vector3<T> Abs(const Vector3<T>& v)
    {
        return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
    }

    template <typename T>
    inline Vector3<T> Floor(const Vector3<T>& v)
    {
        return Vector3<T>(std::floor(v.x), std::floor(v.y), std::floor(v.z));
    }

    template <typename T>
    inline Vector3<T> Ceil(const Vector3<T>& v)
    {
        return Vector3<T>(std::ceil(v.x), std::ceil(v.y), std::ceil(v.z));
    }

    template <typename T>
    inline T Dot(const Vector3<T>& u, const Vector3<T>& v)
    {
        return u.x * v.x + u.y * v.y + u.z * v.z;
    }

    template <typename T>
    inline real NormSquared(const Vector3<T>& v)
    {
        return (real)Dot(v, v);
    }

    template <typename T>
    inline real Norm(const Vector3<T>& v)
    {
        return std::sqrt(NormSquared(v));
    }

    template <typename T>
    inline T Absdot(const Vector3<T>& u, const Vector3<T>& v)
    {
        return std::abs(Dot(u, v));
    }

    template <typename T>
    inline Vector3<T> Cross(const Vector3<T>& u, const Vector3<T>& v)
    {
        T ux = u.x, uy = u.y, uz = u.z;
        T vx = v.x, vy = v.y, vz = v.z;
        return Vector3<T>((uy * vz) - (uz * vy), (uz * vx) - (ux * vz), (ux * vy) - (uy * vx));
    }

    template <typename T>
    inline Vector3<T> Normalize(const Vector3<T>& v)
    {
        ASSERT(v != Vector3<T>());
        return v / Norm(v);
    }

    template <typename T>
    Vector3<T> Min(const Vector3<T>& p1, const Vector3<T>& p2)
    {
        return Vector3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z));
    }
    template <typename T>
    Vector3<T> Max(const Vector3<T>& p1, const Vector3<T>& p2)
    {
        return Vector3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z));
    }

    template <typename T>
    Vector3<T> Permute(const Vector3<T>& v, i32 x, i32 y, i32 z)
    {
        return Vector3<T>(v[x], v[y], v[z]);
    }

    // Given a unit vector v1, constructs an orthonormal set { v1, v2, v3 }. Note this set is only
    // unique up to rotation about v1
    template <typename T>
    inline void CoordinateSystem(const Vector3<T>& v1, Vector3<T>& v2, Vector3<T>& v3)
    {
        if (std::abs(v1.x) > std::abs(v1.y))
            v2 = Vector3<T>(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
        else
            v2 = Vector3<T>(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
        v3 = Cross(v1, v2);
    }

    template <typename T>
    Vector3<T> Lerp(real t, const Vector3<T>& u, const Vector3<T>& v)
    {
        return (1 - t) * u + t * v;
    }

    // Returns a vector parallel to normal and in the same hemiphere oriented along vec
    template <typename T>
    Vector3<T> FaceForward(const Vector3<T>& normal, const Vector3<T>& vec)
    {
        return (Dot(normal, vec) < 0) ? -normal : normal;
    }

    template <typename T>
    real Distance(const Vector3<T>& v1, const Vector3<T>& v2)
    {
        return (v1 - v2).Norm();
    }

    typedef Vector3<real> Vector3f;
    typedef Vector3<i32> Vector3i;

    template <typename T>
    class Vector2
    {
    public:
        Vector2() : x(0), y(0)
        {
        }

        Vector2(T x, T y) : x(x), y(y)
        {
            ASSERT(!HasNaNs());
        }

        Vector2(T arg) : x(arg), y(arg)
        {
            ASSERT(!HasNaNs());
        }

        template <typename U>
        explicit Vector2(const Vector2<U>& vec) : x(vec.x), y(vec.y)
        {
            ASSERT(!HasNaNs());
        }

        constexpr explicit Vector2(const Vector3<T> other) : x(other.x), y(other.y)
        {
            ASSERT(!other.HasNaNs());
        }

        T& operator[](i32 i)
        {
            ASSERT(i >= 0 && i <= 1);
            if (i == 0)
                return x;
            return y;
        }

        T operator[](i32 i) const
        {
            return const_cast<Vector2<T>*>(this)->operator[](i);
        }

        Vector2<T> operator+(const Vector2<T>& other) const
        {
            ASSERT(!other.HasNaNs());
            return Vector2<T>(x + other.x, y + other.y);
        }

        Vector2<T>& operator+=(const Vector2<T>& other)
        {
            ASSERT(!other.HasNaNs());
            x += other.x;
            y += other.y;
            return *this;
        }

        Vector2<T> operator-(const Vector2<T>& other) const
        {
            ASSERT(!other.HasNaNs());
            return Vector2<T>(x - other.x, y - other.y);
        }

        Vector2<T>& operator-=(const Vector2<T>& other)
        {
            ASSERT(!other.HasNaNs());
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vector2<T> operator*(const Vector2<T>& other) const
        {
            ASSERT(!other.HasNaNs());
            return Vector2<T>(x * other.x, y * other.y);
        }

        Vector2<T>& operator*=(const Vector2<T>& other)
        {
            ASSERT(!other.HasNaNs());
            x *= other.x;
            y *= other.y;
            return *this;
        }

        Vector2<T> operator*(T s) const
        {
            ASSERT(!std::isnan(s));
            return Vector2<T>(x * s, y * s);
        }

        Vector2<T>& operator*=(T s)
        {
            ASSERT(!std::isnan(s));
            x *= s;
            y *= s;
            return *this;
        }

        Vector2<T> operator/(T s) const
        {
            ASSERT(s != 0);
            real inv = (real)1 / s;
            return *this * inv;
        }

        Vector2<T>& operator/=(T s)
        {
            ASSERT(s != 0);
            real inv = (real)1 / s;
            *this *= inv;
            return *this;
        }

        Vector2<T> operator-() const
        {
            return Vector2<T>(-x, -y);
        }

        bool operator==(const Vector2<T>& other) const
        {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vector2<T>& other) const
        {
            return !(*this == other);
        }

        T MinComponent() const
        {
            return std::min(x, y);
        }

        T MaxComponent() const
        {
            return std::max(x, y);
        }

        // Returns the index of the component with the largest value
        i32 MaxDimension() const
        {
            return (x > y) ? 0 : 1;
        }

        bool HasNaNs() const
        {
            return std::isnan(x) || std::isnan(y);
        }

    public:
        T x, y;
    };

    // Partial specialization for integer type
    template <>
    inline bool Vector2<i32>::HasNaNs() const
    {
        return false;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& out, const Vector2<T>& other)
    {
        out << "(" << other.x << ", " << other.y << ")";
        return out;
    }

    template <typename T>
    inline Vector2<T> operator*(T s, const Vector2<T>& v)
    {
        return v * s;
    }

    template <typename T>
    inline Vector2<T> Abs(const Vector2<T>& v)
    {
        return Vector2<T>(std::abs(v.x), std::abs(v.y));
    }

    template <typename T>
    inline Vector2<T> Floor(const Vector2<T>& v)
    {
        return Vector2<T>(std::floor(v.x), std::floor(v.y));
    }

    template <typename T>
    inline Vector2<T> Ceil(const Vector2<T>& v)
    {
        return Vector2<T>(std::ceil(v.x), std::ceil(v.y));
    }

    template <typename T>
    inline T Dot(const Vector2<T>& u, const Vector2<T>& v)
    {
        return u.x * v.x + u.y * v.y;
    }

    template <typename T>
    inline real NormSquared(const Vector2<T>& v)
    {
        return (real)Dot(v, v);
    }

    template <typename T>
    inline real Norm(const Vector2<T>& v)
    {
        return std::sqrt(NormSquared(v));
    }

    template <typename T>
    inline T Absdot(const Vector2<T>& u, const Vector2<T>& v)
    {
        return std::abs(Dot(u, v));
    }

    template <typename T>
    inline Vector2<T> Normalize(const Vector2<T>& v)
    {
        ASSERT(v != Vector2<T>());
        return v / Norm(v);
    }

    template <typename T>
    Vector2<T> Min(const Vector2<T>& p1, const Vector2<T>& p2)
    {
        return Vector2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
    }
    template <typename T>
    Vector2<T> Max(const Vector2<T>& p1, const Vector2<T>& p2)
    {
        return Vector2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
    }

    template <typename T>
    Vector2<T> Permute(const Vector2<T>& v, i32 x, i32 y)
    {
        return Vector2<T>(v[x], v[y]);
    }

    // Given a unit vector v1, constructs an orthonormal set { v1, v2 }
    template <typename T>
    inline void CoordinateSystem(const Vector2<T>& v1, Vector2<T>& v2)
    {
        v2 = Vector2<T>(-v1.y, v1.x);
    }

    template <typename T>
    Vector2<T> Lerp(real t, const Vector2<T>& u, const Vector2<T>& v)
    {
        return (1 - t) * u + t * v;
    }

    template <typename T>
    real Distance(const Vector2<T>& v1, const Vector2<T>& v2)
    {
        return (v1 - v2).Norm();
    }

    typedef Vector2<real> Vector2f;
    typedef Vector2<i32> Vector2i;
}