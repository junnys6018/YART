#include "core/imageio.h"
#include <ImfRgba.h>
#include <ImfRgbaFile.h>

namespace yart
{
    void WriteImage(const std::string& filename, const real* pixels, const Bounds2i& outputBounds,
                    const Vector2i& totalResolution)
    {
        using namespace Imf;
        using namespace Imath;

        i32 numPixels = outputBounds.Area();
        i32 totalXRes = totalResolution.x;
        i32 totalYRes = totalResolution.y;
        Vector2i resolution = outputBounds.Diagonal();
        i32 xRes = resolution.x;
        i32 yRes = resolution.y;
        i32 xOffset = outputBounds.m_MinBound.x;
        i32 yOffset = outputBounds.m_MinBound.y;

        Rgba* hrgba = new Rgba[numPixels];
        for (int i = 0; i < numPixels; ++i)
            hrgba[i] = Rgba(pixels[3 * i], pixels[3 * i + 1], pixels[3 * i + 2]);

        // OpenEXR uses inclusive pixel bounds.
        Box2i displayWindow(V2i(0, 0), V2i(totalXRes - 1, totalYRes - 1));
        Box2i dataWindow(V2i(xOffset, yOffset), V2i(xOffset + xRes - 1, yOffset + yRes - 1));

        try
        {
            RgbaOutputFile file(filename.c_str(), displayWindow, dataWindow, WRITE_RGB);
            file.setFrameBuffer(hrgba - xOffset - yOffset * xRes, 1, xRes);
            file.writePixels(yRes);
        }
        catch (const std::exception& exc)
        {
            LOG_ERROR("Error writing \"{}\": {}", filename.c_str(), exc.what());
        }

        delete[] hrgba;
    }
}
