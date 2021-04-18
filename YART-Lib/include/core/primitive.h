#pragma once
#include "math/boundingbox.h"
#include "math/ray.h"
#include "core/interaction.h"
#include "geometry/geometry.h"

namespace yart
{
	class AbstractPrimitive
	{
	public:
		virtual Bounds3f WorldBound() const = 0;
		virtual bool IntersectRay(const Ray& ray, SurfaceInteraction* surfaceInt) const = 0;
		virtual bool IntersectRay(const Ray& ray) const = 0;

		// Return nullptr if the primitive is not emmisive
		//virtual const AreaLight* GetAreaLight() const = 0;

		//virtual const Material* GetMaterial() const = 0;

		//virtual void ComputeScatteringFuctions(SurfaceInteraction* surfaceInt, MemoryArena& arena,
		//									   TransportMode mode, bool allowMultipleLobes) const = 0;


	public:
		const AbstractPrimitive* m_HitPrimitive = nullptr;
	};

	class GeometricPrimitive : public AbstractPrimitive
	{
	public:
		GeometricPrimitive(const std::shared_ptr<AbstractGeometry>& geometry) 
			: m_Geometry(geometry)
		{

		}

		virtual Bounds3f WorldBound() const override;
		virtual bool IntersectRay(const Ray& ray, SurfaceInteraction* surfaceInt = nullptr) const override;
		virtual bool IntersectRay(const Ray& ray) const override;

	private:
		std::shared_ptr<AbstractGeometry> m_Geometry;
		//std::shared_ptr<Material> m_Material;
		//std::shared_ptr<AreaLight> m_Light;

		// TODO: medium interface
	};

	// TODO: TransformPrimitive (section 4.1.2)
}