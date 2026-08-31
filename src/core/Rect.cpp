#include "Rect.h"

#include <algorithm>

Rect &Rect::extend(const Rect &other)
{
    const int xmin1 = x;
    const int xmax1 = x + w;
    const int ymin1 = y;
    const int ymax1 = y + h;

    const int xmin2 = other.x;
    const int xmax2 = other.x + other.w;
    const int ymin2 = other.y;
    const int ymax2 = other.y + other.h;

    x = std::min(xmin1, xmin2);
    y = std::min(ymin1, ymin2);
    w = std::max(xmax1, xmax2) - x;
    h = std::max(ymax1, ymax2) - y;

    return *this;
}

Rect &Rect::cropTo(const Rect &other)
{
    const int xmin1 = x;
    const int xmax1 = x + w;
    const int ymin1 = y;
    const int ymax1 = y + h;

    const int xmin2 = other.x;
    const int xmax2 = other.x + other.w;
    const int ymin2 = other.y;
    const int ymax2 = other.y + other.h;

    x = std::max(xmin1, xmin2);
    y = std::max(ymin1, ymin2);
    w = std::min(xmax1, xmax2) - x;
    h = std::min(ymax1, ymax2) - y;

    return *this;
}

Rect Rect::getUnion(Rect a, const Rect &b)
{
    return a.extend(b);
}

Rect Rect::getIntersection(Rect a, const Rect &b)
{
    return a.cropTo(b);
}
