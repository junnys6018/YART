#include "pch.h"
#include "geometry/sphere.h"
#include "core/interaction.h"

namespace yart
{
	Bounds3f Sphere::ObjectBound() const
	{
		// TODO: A tigher bound can be made for phi < 3pi/2
		return Bounds3f(Vector3f(-m_Radius, -m_Radius, m_zMin), Vector3f(m_Radius, m_Radius, m_zMax));
	}

	bool Sphere::IntersectRay(const Ray& ray, real* tHit, SurfaceInteraction* surfaceInt,
							  bool testAlphaTexture) const
	{
		// TODO: account for floating point error
		Ray objSpaceRay = m_WorldToObject->AppRay(ray);

		// Calculate quadratic equation coefficents
		real a = NormSquared(objSpaceRay.d);
		real b = 2 * Dot(objSpaceRay.d, objSpaceRay.o);
		real c = NormSquared(objSpaceRay.o) - m_Radius * m_Radius;

		// Solve quadratic
		real t1, t2;
		if (!Quadratic(a, b, c, &t1, &t2))
		{
			return false;
		}

		// Check that t1 and t2 is within the rays time bound
		if (t1 > ray.m_Tmax || t2 <= 0)
		{
			return false;
		}

		// Get the time of the first hit
		real tShapeHit = t1;
		if (tShapeHit <= 0)
		{
			tShapeHit = t2;
			if (tShapeHit > ray.m_Tmax)
				return false;
		}

		// Get the point in object space of the first hit
		Vector3f pHit = objSpaceRay(tShapeHit);

		real phi = std::atan2(pHit.y, pHit.x);
		if (phi < 0)
			phi += 2 * Pi;

		// Check if the hit point is out of the spheres bounds
		if ((m_zMin > -m_Radius && pHit.z < m_zMin) || (m_zMax < m_Radius && pHit.z > m_zMax) || phi > m_PhiMax)
		{
			// First hit point is out of the sphers bounds
			// Check the second hit point
			if (tShapeHit == t2)
				return false;
			if (t2 > ray.m_Tmax)
				return false;

			tShapeHit = t2;
			pHit = objSpaceRay(tShapeHit);
			phi = std::atan2(pHit.y, pHit.x);
			if (phi < 0)
				phi += 2 * Pi;

			if ((m_zMin > -m_Radius && pHit.z < m_zMin) || (m_zMax < m_Radius && pHit.z > m_zMax) ||
				phi > m_PhiMax)
			{
				return false;
			}
		}

		// Find parametric (u,v) representation of hit point
		real u = phi / m_PhiMax;
		real theta = std::acos(pHit.z / m_Radius);
		real deltaTheta = m_ThetaMax - m_ThetaMin;
		real v = (theta - m_ThetaMin) / deltaTheta;
		
		// Compute partial derivates dpdu and dpdv
		real zRadius = std::sqrt(pHit.x * pHit.x + pHit.y * pHit.y);
		real invZRadius = 1 / zRadius;
		real cosPhi = pHit.x * invZRadius;
		real sinPhi = pHit.y * invZRadius;
		Vector3f dpdu(-m_PhiMax * pHit.y, m_PhiMax * pHit.x, 0);
		Vector3f dpdv = deltaTheta * Vector3f(pHit.z * cosPhi, pHit.z * sinPhi, -m_Radius * std::sin(theta));

		// Compute partial derivates dndu and dndv

		Vector3f dpdu2 = -m_PhiMax * m_PhiMax* Vector3f(pHit.x, pHit.y, 0);
		Vector3f dpduv = deltaTheta * pHit.z * m_PhiMax * Vector3f(-sinPhi, cosPhi, 0);
		Vector3f dpdv2 = -deltaTheta * deltaTheta * pHit;

		// First fundamental forms
		real E = NormSquared(dpdu);
		real F = Dot(dpdu, dpdv);
		real G = NormSquared(dpdv);

		// Second fundamental forms
		Vector3f normal = Normalize(Cross(dpdu, dpdv));
		real e = Dot(normal, dpdu2);
		real f = Dot(normal, dpduv);
		real g = Dot(normal, dpdv2);

		real invEGF2 = 1 / (E * G - F * F);
		Vector3f dndu = (f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv;
		Vector3f dndv = (g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv;

		if (surfaceInt)
			*surfaceInt = m_ObjectToWorld->AppSI(SurfaceInteraction(pHit, Vector3f{}, Vector2f(u, v), -ray.d, dpdu,
																	dpdv, dndu, dndv, ray.m_Time, this));
		if (tHit)
			*tHit = tShapeHit;

		return true;
	}

	real Sphere::SurfaceArea() const
	{
		return m_PhiMax * m_Radius * (m_zMax - m_zMin);
	}
}