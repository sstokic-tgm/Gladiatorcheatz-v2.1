#pragma once

#include <sstream>

#define CHECK_VALID(_v) 0
#define Assert(_exp) ((void)0)

inline float sqrt2(float sqr)
{
	float root = 0;

	__asm
	{
		sqrtss xmm0, sqr
		movss root, xmm0
	}

	return root;
}

class Vector
{
	
public:

	__inline Vector (void)
    {
        Invalidate();
    }
	
    Vector(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }
	
    Vector(const float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        x = ix; y = iy; z = iz;
    }

	__inline void Mul(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}

	__inline void MulAdd(const Vector& a, const Vector& b, float scalar) {
		x = a.x + b.x * scalar;
		y = a.y + b.y * scalar;
		z = a.z + b.z * scalar;
	}
	
	__inline bool IsValid() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
	
    void __inline Invalidate()
    {
        x = y = z = std::numeric_limits<float>::infinity();
    }

    float &operator[](int i)
    {
        return ((float*)this)[i];
    }
	
    float operator[](int i) const
    {
        return ((float*)this)[i];
    }

	bool __inline IsZero()
	{
		return (!x && !y && !z);
	}

    void __inline Zero()
    {
        x = y = z = 0.0f;
    }

    bool operator==(const Vector &src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
	
    bool operator!=(const Vector &src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }

    Vector &operator+=(const Vector &v)
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
	
    Vector &operator-=(const Vector &v)
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
	
    Vector &operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
	
    Vector &operator*=(const Vector &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
	
    Vector &operator/=(const Vector &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
	
    Vector &operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
	
    Vector &operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
	
    Vector &operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }

    void NormalizeInPlace()
    {
        *this = Normalized();
    }
	
    Vector Normalized() const
    {
        Vector res = *this;
        float l = res.Length();
        if(l != 0.0f) {
            res /= l;
        } else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }

	float Normalize() const
	{
		Vector res = *this;
		float l = res.Length();
		if (l != 0.0f)
		{
			res /= l;
		}
		else
		{
			res.x = res.y = res.z = 0.0f;
		}
		return l;
	}

    float DistTo(const Vector &vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.Length();
    }
	
    float DistToSqr(const Vector &vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.LengthSqr();
    }
	
    float Dot(const Vector &vOther) const
    {
        return (x*vOther.x + y*vOther.y + z*vOther.z);
    }

	void VectorCrossProduct(const Vector &a, const Vector &b, Vector &result)
	{
		result.x = a.y*b.z - a.z*b.y;
		result.y = a.z*b.x - a.x*b.z;
		result.z = a.x*b.y - a.y*b.x;
	}

	Vector Cross(const Vector &vOther)
	{
		Vector res;
		VectorCrossProduct(*this, vOther, res);
		return res;
	}
	
    float Length() const
    {
        return sqrt(x*x + y*y + z*z);
    }
	
    float LengthSqr(void) const
    {
        return (x*x + y*y + z*z);
    }
	
    float Length2D() const
    {
        return sqrt(x*x + y*y);
    }

	Vector Angle(Vector *up = 0)
	{
		if (!x && !y)
			return Vector(0, 0, 0);

		float roll;

		if (up)
		{
			Vector left = (*up).Cross(*this);
			roll = atan2f(left.z, (left.y * x) - (left.x * y)) * 180.0f / 3.14159265358979323846f;
		}

		return Vector(atan2f(-z, sqrt2(x*x + y*y)) * 180.0f / 3.14159265358979323846f, atan2f(y, x) * 180.0f / 3.14159265358979323846f, roll);
	}

    Vector &operator=(const Vector &vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }

    Vector Vector::operator-(void) const
    {
        return Vector(-x, -y, -z);
    }
	
    Vector Vector::operator+(const Vector &v) const
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }

	Vector Vector::operator+(float fl) const
	{
		return Vector(x + fl, y + fl, z + fl);
	}
	
    Vector Vector::operator-(const Vector &v) const
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
	
	Vector Vector::operator-(float fl) const
	{
		return Vector(x - fl, y - fl, z - fl);
	}

    Vector Vector::operator*(float fl) const
    {
        return Vector(x * fl, y * fl, z * fl);
    }
	
    Vector Vector::operator*(const Vector &v) const
    {
        return Vector(x * v.x, y * v.y, z * v.z);
    }
	
    Vector Vector::operator/(float fl) const
    {
        return Vector(x / fl, y / fl, z / fl);
    }
	
    Vector Vector::operator/(const Vector &v) const
    {
        return Vector(x / v.x, y / v.y, z / v.z);
    }

    float x, y, z;
};

inline Vector operator*(float lhs, const Vector &rhs)
{
    return rhs * lhs;
}

inline Vector operator/(float lhs, const Vector &rhs)
{
    return rhs / lhs;
}

class __declspec(align(16)) VectorAligned : public Vector
{
	
public:

    inline VectorAligned(void) {};
    inline VectorAligned(float X, float Y, float Z)
    {
        Init(X, Y, Z);
    }

public:

    explicit VectorAligned(const Vector &vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
    }

    VectorAligned &operator=(const Vector &vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    VectorAligned &operator=(const VectorAligned &vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    float w;
};

typedef unsigned __int32		uint32;

inline uint32 const FloatBits(const float &f)
{
	union Convertor_t
	{
		float f;
		uint32 ul;
	}tmp;
	tmp.f = f;
	return tmp.ul;
}

inline bool IsFinite(const float &f)
{
#if _X360
	return f == f && fabs(f) <= FLT_MAX;
#else
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
#endif
}

inline void VectorMultiply(const Vector &a, float b, Vector &c)
{
	CHECK_VALID(a);
	Assert(IsFinite(b));
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
}

inline void VectorMA(const Vector &start, float scale, const Vector &direction, Vector &dest)
{
	CHECK_VALID(start);
	CHECK_VALID(direction);

	dest.x = start.x + scale * direction.x;
	dest.y = start.y + scale * direction.y;
	dest.z = start.z + scale * direction.z;
}

inline void VectorAdd(const Vector &a, const Vector &b, Vector &c)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

inline void VectorSubtract(const Vector &a, const Vector &b, Vector &c)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}