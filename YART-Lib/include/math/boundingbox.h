#pragma once
#include "core/yart.h"
#include "math/ray.h"
#include "math/util.h"
#include "math/vector.h"

namespace yart
{
	template <typename T>
	class Bounds3
	{
	public:
		// Default construct an empty box
		Bounds3() : m_MinBound(std::numeric_limits<T>::max()), m_MaxBound(std::numeric_limits<T>::lowest())
		{
		}

		// Construct a bound that encloses a single point
		Bounds3(const Vector3<T>& p) : m_MinBound(p), m_MaxBound(p)
		{
		}

		Bounds3(const Vector3<T>& p1, const Vector3<T>& p2) : m_MinBound(Min(p1, p2)), m_MaxBound(Max(p1, p2))
		{
		}

		template <typename U>
		explicit Bounds3(const Bounds3<U>& bound) : m_MinBound(bound.m_MinBound), m_MaxBound(bound.m_MaxBound)
		{
		}

		Vector3<T>& operator[](i32 i)
		{
			ASSERT(i == 0 || i == 1);
			return (i == 0) ? m_MinBound : m_MaxBound;
		}

		const Vector3<T>& operator[](i32 i) const
		{
			return const_cast<Bounds3<T>*>(this)->operator[](i);
		}

		Vector3<T> Corner(i32 corner) const
		{
			return Vector3<T>((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y, (*this)[(corner & 4) ? 1 : 0].z);
		}

		Vector3<T> Diagonal() const
		{
			return m_MaxBound - m_MinBound;
		}

		T SurfaceArea() const
		{
			Vector3<T> d = Diagonal();
			return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
		}

		T Volume() const
		{
			Vector3<T> d = Diagonal();
			return d.x * d.y * d.z;
		}

		// Returns the index of which of the three axis is longest
		i32 MaximumExtent() const
		{
			Vector3<T> d = Diagonal();
			if (d.x > d.y && d.x > d.z)
				return 0;
			else if (d.y > d.z)
				return 1;
			else
				return 2;
		}

		Vector3<T> Lerp(const Vector3<T>& t) const
		{
			return Vector3<T>(yart::Lerp(t.x, m_MinBound.x, m_MaxBound.x), yart::Lerp(t.y, m_MinBound.y, m_MaxBound.y),
							  yart::Lerp(t.z, m_MinBound.z, m_MaxBound.z));
		}

		// Returns the interpolated position of a point relative to the corners of the box,
		// e.g. a point at the minimum corner has offset (0,0,0), a point in the middle of the box has offset (0.5,0.5,0.5)
		Vector3<T> Offset(const Vector3<T>& p) const
		{
			Vector3<T> o = p - m_MinBound;
			if (m_MaxBound.x > m_MinBound.x)
				o.x /= m_MaxBound.x - m_MinBound.x;
			if (m_MaxBound.y > m_MinBound.y)
				o.y /= m_MaxBound.y - m_MinBound.y;
			if (m_MaxBound.z > m_MinBound.z)
				o.z /= m_MaxBound.z - m_MinBound.z;
			return o;
		}

		// Returns the center and radius of the smallest sphere that contains the bound
		void BoundingSphere(Vector3<T>* center, real* radius) const
		{
			*center = (m_MinBound + m_MaxBound) / 2;
			*radius = Inside(center, *this) ? Distance(center, m_MaxBound) : 0;
		}

		bool IntersectRay(const Ray& ray, real* hitt1, real* hitt2) const
		{
			real t1 = 0;
			real t2 = ray.m_Tmax;

			// For each axis aligned "slab"
			for (i32 i = 0; i < 3; ++i)
			{
				real invRayDir = 1 / ray.d[i];
				real tNear = (m_MinBound[i] - ray.o[i]) * invRayDir;
				real tFar = (m_MaxBound[i] - ray.o[i]) * invRayDir;

				if (tNear > tFar)
					std::swap(tNear, tFar);

				tFar *= 1 + 2 * gamma(3);

				t1 = tNear > t1 ? tNear : t1;
				t2 = tFar < t2 ? tFar : t2;
				if (t1 > t2)
					return false;
			}

			if (hitt1)
				*hitt1 = t1;
			if (hitt2)
				*hitt2 = t2;

			return true;
		}

		// Optimised overload of IntersectRay() that avoids the std::swap and inverse calculation
		// Note: If the ray origin in a yz plane and the ray direction is parallel to the yz plane then this function
		// will return false. If the ray is aligned with the xy or xz plane, then the function will return true
		bool IntersectRay(const Ray& ray, const Vector3f& invRayDir, const i32 dirIsNeg[3]) const
		{
			const Bounds3<real>& bounds = *this;
			// Check for ray intersection against x and y slabs
			real tMin = (bounds[dirIsNeg[0]].x - ray.o.x) * invRayDir.x;
			real tMax = (bounds[1 - dirIsNeg[0]].x - ray.o.x) * invRayDir.x;
			real tyMin = (bounds[dirIsNeg[1]].y - ray.o.y) * invRayDir.y;
			real tyMax = (bounds[1 - dirIsNeg[1]].y - ray.o.y) * invRayDir.y;

			tMax *= 1 + 2 * gamma(3);
			tyMax *= 1 + 2 * gamma(3);
			if (tMin > tyMax || tyMin > tMax)
				return false;
			if (tyMin > tMin)
				tMin = tyMin;
			if (tyMax < tMax)
				tMax = tyMax;

			// Check for ray intersection against z slab
			real tzMin = (bounds[dirIsNeg[2]].z - ray.o.z) * invRayDir.z;
			real tzMax = (bounds[1 - dirIsNeg[2]].z - ray.o.z) * invRayDir.z;

			tzMax *= 1 + 2 * gamma(3);
			if (tMin > tzMax || tzMin > tMax)
				return false;
			if (tzMin > tMin)
				tMin = tzMin;
			if (tzMax < tMax)
				tMax = tzMax;
			return (tMin < ray.m_Tmax) && (tMax > 0);
		}

	public:
		Vector3<T> m_MinBound;
		Vector3<T> m_MaxBound;
	};

	template <typename T>
	std::ostream& operator<<(std::ostream& out, const Bounds3<T>& bound)
	{
		out << "{min: " << bound.m_MinBound << ", max: " << bound.m_MaxBound << "}";
		return out;
	}

	// Returns the smallest bound that contains both the bound and the point
	template <typename T>
	Bounds3<T> Union(const Bounds3<T>& bound, const Vector3<T>& point)
	{
		return Bounds3<T>(Min(bound.m_MinBound, point), Max(bound.m_MaxBound, point));
	}

	template <typename T>
	Bounds3<T> Union(const Bounds3<T>& b1, const Bounds3<T>& b2)
	{
		return Bounds3<T>(Min(b1.m_MinBound, b2.m_MinBound), Max(b1.m_MaxBound, b2.m_MaxBound));
	}

	template <typename T>
	Bounds3<T> Intersect(const Bounds3<T>& b1, const Bounds3<T>& b2)
	{
		return Bounds3<T>(Max(b1.m_MinBound, b2.m_MinBound), Min(b1.m_MaxBound, b2.m_MaxBound));
	}

	template <typename T>
	bool Overlaps(const Bounds3<T>& b1, const Bounds3<T>& b2)
	{
		bool x = (b1.m_MaxBound.x >= b2.m_MinBound.x) && (b1.m_MinBound.x <= b2.m_MaxBound.x);
		bool y = (b1.m_MaxBound.y >= b2.m_MinBound.y) && (b1.m_MinBound.y <= b2.m_MaxBound.y);
		bool z = (b1.m_MaxBound.z >= b2.m_MinBound.z) && (b1.m_MinBound.z <= b2.m_MaxBound.z);
		return x && y && z;
	}

	template <typename T>
	bool Inside(const Vector3<T>& p, const Bounds3<T>& b)
	{
		return (p.x >= b.m_MinBound.x && p.x <= b.m_MaxBound.x && p.y >= b.m_MinBound.y && p.y <= b.m_MaxBound.y &&
				p.z >= b.m_MinBound.z && p.z <= b.m_MaxBound.z);
	}

	// Do not consider points on the upper boundary to be inside the bounds. Useful for integer-typed bounds
	template <typename T>
	bool InsideExclusive(const Vector3<T>& p, const Bounds3<T>& b)
	{
		return (p.x >= b.m_MinBound.x && p.x < b.m_MaxBound.x && p.y >= b.m_MinBound.y && p.y < b.m_MaxBound.y &&
				p.z >= b.m_MinBound.z && p.z < b.m_MaxBound.z);
	}

	template <typename T, typename U>
	inline Bounds3<T> Expand(const Bounds3<T>& b, U delta)
	{
		return Bounds3<T>(b.m_MinBound - Vector3<T>(delta), b.m_MaxBound + Vector3<T>(delta));
	}

	typedef Bounds3<real> Bounds3f;
	typedef Bounds3<i32> Bounds3i;

	template <typename T>
	class Bounds2
	{
	public:
		// Default construct an empty box
		Bounds2() : m_MinBound(std::numeric_limits<T>::max()), m_MaxBound(std::numeric_limits<T>::lowest())
		{
		}

		// Construct a bound that encloses a single point
		Bounds2(const Vector2<T>& p) : m_MinBound(p), m_MaxBound(p)
		{
		}

		Bounds2(const Vector2<T>& p1, const Vector2<T>& p2) : m_MinBound(Min(p1, p2)), m_MaxBound(Max(p1, p2))
		{
		}

		template <typename U>
		explicit Bounds2(const Bounds2<U>& bound) : m_MinBound(bound.m_MinBound), m_MaxBound(bound.m_MaxBound)
		{
		}

		Vector2<T>& operator[](i32 i)
		{
			ASSERT(i == 0 || i == 1);
			return (i == 0) ? m_MinBound : m_MaxBound;
		}

		const Vector2<T>& operator[](i32 i) const
		{
			return const_cast<Bounds2<T>*>(this)->operator[](i);
		}

		Vector2<T> Corner(i32 corner) const
		{
			return Vector2<T>((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y);
		}

		Vector2<T> Diagonal() const
		{
			return m_MaxBound - m_MinBound;
		}

		T Perimeter() const
		{
			Vector2<T> d = Diagonal();
			return 2 * (d.x + d.y);
		}

		T Area() const
		{
			Vector2<T> d = Diagonal();
			return d.x * d.y;
		}

		// Returns the index of which of the two axis is longest
		i32 MaximumExtent() const
		{
			Vector2<T> d = Diagonal();
			if (d.x > d.y)
				return 0;
			return 1;
		}

		Vector2<T> Lerp(const Vector2<T>& t) const
		{
			return Vector2<T>(yart::Lerp(t.x, m_MinBound.x, m_MaxBound.x), yart::Lerp(t.y, m_MinBound.y, m_MaxBound.y));
		}

		// Returns the continuous position of a point relative to the corners of the box,
		// e.g. a point at the minimum corner has offset (0,0), a point in the middle of the box has offset (0.5,0.5)
		Vector2<T> Offset(const Vector2<T>& p) const
		{
			Vector2<T> o = p - m_MinBound;
			if (m_MaxBound.x > m_MinBound.x)
				o.x /= m_MaxBound.x - m_MinBound.x;
			if (m_MaxBound.y > m_MinBound.y)
				o.y /= m_MaxBound.y - m_MinBound.y;
			return o;
		}

		// Returns the center and radius of the smallest circle that contains the bound
		void BoundingCircle(Vector2<T>& center, real& radius) const
		{
			center = (m_MinBound + m_MaxBound) / 2;
			radius = Inside(center, *this) ? Distance(center, m_MaxBound) : 0;
		}

	public:
		Vector2<T> m_MinBound;
		Vector2<T> m_MaxBound;
	};

	template <typename T>
	std::ostream& operator<<(std::ostream& out, const Bounds2<T>& bound)
	{
		out << "{ min: " << bound.m_MinBound << ", max: " << bound.m_MaxBound << " }";
		return out;
	}

	// Returns the smallest bound that contains both the bound and the point
	template <typename T>
	Bounds2<T> Union(const Bounds2<T>& bound, const Vector2<T>& point)
	{
		return Bounds2<T>(Min(bound.m_MinBound, point), Max(bound.m_MaxBound, point));
	}

	template <typename T>
	Bounds2<T> Union(const Bounds2<T>& b1, const Bounds2<T>& b2)
	{
		return Bounds2<T>(Min(b1.m_MinBound, b2.m_MinBound), Max(b1.m_MaxBound, b2.m_MaxBound));
	}

	template <typename T>
	Bounds2<T> Intersect(const Bounds2<T>& b1, const Bounds2<T>& b2)
	{
		return Bounds2<T>(Max(b1.m_MinBound, b2.m_MinBound), Min(b1.m_MaxBound, b2.m_MaxBound));
	}

	template <typename T>
	bool Overlaps(const Bounds2<T>& b1, const Bounds2<T>& b2)
	{
		bool x = (b1.m_MaxBound.x >= b2.m_MinBound.x) && (b1.m_MinBound.x <= b2.m_MaxBound.x);
		bool y = (b1.m_MaxBound.y >= b2.m_MinBound.y) && (b1.m_MinBound.y <= b2.m_MaxBound.y);
		return x && y;
	}

	template <typename T>
	bool Inside(const Vector2<T>& p, const Bounds2<T>& b)
	{
		return (p.x >= b.m_MinBound.x && p.x <= b.m_MaxBound.x && p.y >= b.m_MinBound.y && p.y <= b.m_MaxBound.y);
	}

	// Do not consider points on the upper boundary to be inside the bounds. Useful for integer-typed bounds
	template <typename T>
	bool InsideExclusive(const Vector2<T>& p, const Bounds2<T>& b)
	{
		return (p.x >= b.m_MinBound.x && p.x < b.m_MaxBound.x && p.y >= b.m_MinBound.y && p.y < b.m_MaxBound.y);
	}

	template <typename T, typename U>
	inline Bounds2<T> Expand(const Bounds2<T>& b, U delta)
	{
		return Bounds2<T>(b.m_MinBound - Vector2<T>(delta), b.m_MaxBound + Vector2<T>(delta));
	}

	typedef Bounds2<real> Bounds2f;
	typedef Bounds2<i32> Bounds2i;

	// Iterate through all the integer coordinates of a bounding box, (excluding maximum extent)
	class Bounds2iIterator : public std::forward_iterator_tag
	{
	public:
		Bounds2iIterator(const Bounds2i& b, const Vector2i& pt) : m_Point(pt), m_Bounds(&b)
		{
		}

		Bounds2iIterator operator++()
		{
			Advance();
			return *this;
		}

		Bounds2iIterator operator++(i32)
		{
			Bounds2iIterator old = *this;
			Advance();
			return old;
		}

		bool operator==(const Bounds2iIterator& other) const
		{
			return m_Point == other.m_Point && m_Bounds == other.m_Bounds;
		}

		bool operator!=(const Bounds2iIterator& other) const
		{
			return !(*this == other);
		}

		const Vector2i& operator*() const
		{
			return m_Point;
		}

	private:
		void Advance()
		{
			m_Point.x++;
			if (m_Point.x == m_Bounds->m_MaxBound.x)
			{
				m_Point.x = m_Bounds->m_MinBound.x;
				m_Point.y++;
			}
		}
		Vector2i m_Point;
		const Bounds2i* m_Bounds;
	};

	inline Bounds2iIterator begin(const Bounds2i& b)
	{
		return Bounds2iIterator(b, b.m_MinBound);
	}

	inline Bounds2iIterator end(const Bounds2i& b)
	{
		// Normally, the ending point is at the minimum x value and one past
		// the last valid y value.
		Vector2i end(b.m_MinBound.x, b.m_MaxBound.y);
		// However, if the bounds are degenerate, override the end point to
		// equal the start point so that any attempt to iterate over the bounds
		// exits out immediately.
		if (b.m_MinBound.x >= b.m_MaxBound.x || b.m_MinBound.y >= b.m_MaxBound.y)
			end = b.m_MinBound;
		return Bounds2iIterator(b, end);
	}
}