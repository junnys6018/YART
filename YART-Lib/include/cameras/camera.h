#pragma once
#include "core/film.h"
#include "math/ray.h"
#include "math/transform.h"
#include "math/vector.h"

namespace yart
{
    struct CameraSample
    {
        Vector2f m_FilmPoint; // In Raster Space coordinates
        Vector2f m_LensPoint; // In [0, 1) x [0, 1)
    };

    class AbstractCamera
    {
    public:
        AbstractCamera(const Transform& cameraToWorld, Film* film) : m_CameraToWorld(cameraToWorld), m_Film(film)
        {
        }

        virtual real GenerateRay(const CameraSample& sample, Ray* ray) const = 0;

        virtual real GenerateRayDifferential(const CameraSample& sample, RayDifferential* rayDifferential) const
        {
            real weight = GenerateRay(sample, rayDifferential);

            // Generate X
            CameraSample shift = sample;
            shift.m_FilmPoint.x++;
            Ray rayShift;
            real weightShift = GenerateRay(shift, &rayShift);
            if (weightShift == 0)
                return 0;

            rayDifferential->m_xDirection = rayShift.d;
            rayDifferential->m_xOrigin = rayShift.o;

            // Generate Y
            shift.m_FilmPoint.x--;
            shift.m_FilmPoint.y++;
            weightShift = GenerateRay(shift, &rayShift);
            if (weightShift == 0)
                return 0;

            rayDifferential->m_yDirection = rayShift.d;
            rayDifferential->m_yOrigin = rayShift.o;

            return weight;
        }

        Transform m_CameraToWorld;
        Film* m_Film;
    };

    class ProjectiveCamera : public AbstractCamera
    {
    public:
        ProjectiveCamera(const Transform& cameraToWorld, const Transform& cameraToScreen, const Bounds2f& screenWindow,
                         Film* film)
            : AbstractCamera(cameraToWorld, film), m_CameraToScreen(cameraToScreen)
        {
            m_ScreenToRaster = Scale({(real)film->m_Resolution.x, (real)film->m_Resolution.y, 1}) *
                               Scale({1 / (screenWindow.m_MaxBound.x - screenWindow.m_MinBound.x),
                                      -1 / (screenWindow.m_MaxBound.y - screenWindow.m_MinBound.y), 1}) *
                               Translate({-screenWindow.m_MinBound.x, -screenWindow.m_MaxBound.y, 0});
            m_RasterToScreen = Inverse(m_ScreenToRaster);
            m_RasterToCamera = Inverse(m_CameraToScreen) * m_RasterToScreen;
        }

    protected:
        Transform m_CameraToScreen, m_RasterToCamera;
        Transform m_ScreenToRaster, m_RasterToScreen;
    };
}