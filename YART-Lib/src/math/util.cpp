#include "math/util.h"

namespace yart
{
    bool Quadratic(real a, real b, real c, real* t1, real* t2)
    {
        // Use doubles regardless of the type of real to mitigate floating point error
        double delta = (double)b * (double)b - 4 * (double)a * (double)c;

        if (delta < 0)
            return false;

        double rootDelta = std::sqrt(delta);

        double q;
        if (b < 0)
        {
            q = -0.5 * (b - rootDelta);
        }
        else
        {
            q = -0.5 * (b + rootDelta);
        }

        if (t1)
            *t1 = q / a;
        if (t2)
            *t2 = c / q;

        if (t1 && t2 && *t1 > *t2)
        {
            std::swap(*t1, *t2);
        }

        return true;
    }
}