#pragma once
#include "math/vector.h"

namespace yart
{
	class Ray
	{
	public:
		Ray()
			: m_Tmax(Infinity), m_Time(0)
		{
		}

		Ray(const Vector3f& o, const Vector3f& d, real tmax = Infinity, real time = 0)
			: o(o), d(d), m_Tmax(tmax), m_Time(time)
		{
		}

		Vector3f operator()(real t) const
		{
			return o + d * t;
		}

	public:
		Vector3f o;
		Vector3f d;

		mutable real m_Tmax;
		real m_Time;
	};

	std::ostream& operator<<(std::ostream& out, const Ray& ray)
	{
		out << "{ o: " << ray.o << ", d: " << ray.d << " }";
		return out;
	}

	// Contains infomation about additional rays offset by a small delta from the original ray
	// Used by texture sampling algorithms to mitigate aliasing
	class RayDifferential : public Ray
	{
	public:
		RayDifferential()
			: m_HasDifferentials(false)
		{
		}

		RayDifferential(const Vector3f& o, const Vector3f& d, real tmax = Infinity, real time = 0)
			: Ray(o, d, tmax, time), m_HasDifferentials(false)
		{
		}

		RayDifferential(const Ray& ray)
			: Ray(ray), m_HasDifferentials(false)
		{
		}

		void ScaleDifferentials(real s)
		{
			m_xOrigin = o + (m_xOrigin - o) * s;
			m_yOrigin = o + (m_yOrigin - o) * s;
			m_xDirection = d + (m_xDirection - d) * s;
			m_yDirection = d + (m_yDirection - d) * s;
		}

	public:
		bool m_HasDifferentials;

		Vector3f m_xOrigin, m_yOrigin;
		Vector3f m_xDirection, m_yDirection;
	};
}