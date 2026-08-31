#pragma once

struct Rect
{
    int x, y, w, h;

    Rect &extend(const Rect &other);
    Rect &cropTo(const Rect &other);

    static Rect getUnion(Rect a, const Rect &b);
    static Rect getIntersection(Rect a, const Rect &b);

    bool operator==(const Rect &other) const = default;
};
