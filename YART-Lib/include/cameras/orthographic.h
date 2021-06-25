#pragma once

#include "camera.h"
namespace yart
{
	class OrthographicCamera : public ProjectiveCamera
	{
	public:
		OrthographicCamera(const Transform& cameraToWorld, const Bounds2f& screenWindow, Film* film)
			: ProjectiveCamera(cameraToWorld, Orthographic(0, 1), screenWindow, film),
			  m_dxCamera(m_RasterToCamera.AppVec(Vector3f{1, 0, 0})), m_dyCamera(m_RasterToCamera.AppVec(Vector3f{0, 1, 0}))
		{
		}

		virtual real GenerateRay(const CameraSample& sample, Ray* ray) const override
		{
			Vector3f filmPoint{sample.m_FilmPoint.x, sample.m_FilmPoint.y, 0};
			Vector3f cameraPoint = m_RasterToCamera.AppPoint(filmPoint);

			ray->o = m_CameraToWorld.AppPoint(cameraPoint);
			ray->d = m_CameraToWorld.AppVec(Vector3f{0, 0, 1});
		}
	private:
		Vector3f m_dxCamera, m_dyCamera; // To be used in GenerateRayDifferential()
	};
}