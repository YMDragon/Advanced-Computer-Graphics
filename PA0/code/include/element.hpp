#pragma once

#include <image.hpp>
#include <algorithm>
#include <queue>
#include <cstdio>

class Element
{
public:
    virtual void draw(Image &img) = 0;
    virtual ~Element() = default;
};

class Line : public Element
{

public:
    int xA, yA;
    int xB, yB;
    Vector3f color;
    void draw(Image &img) override
    {
        // TODO: Implement Bresenham Algorithm
        if (std::abs(xA - xB) >= std::abs(yA - yB))
        {
            if (xA > xB)
                std::swap(xA, xB), std::swap(yA, yB);
            int x, y, dx, dy, e, type = (yA <= yB ? 1 : -1);
            dx = xB - xA, dy = yB - yA, e = -type * dx, x = xA, y = yA;
            for (int i = 0; i <= dx; i++)
            {
                img.SetPixel(x, y, color);
                x++, e += 2 * dy;
                if (e * type >= 0)
                    y += type, e -= 2 * type * dx;
            }
        }
        else
        {
            if (yA > yB)
                std::swap(xA, xB), std::swap(yA, yB);
            int x, y, dx, dy, e, type = (xA <= xB ? 1 : -1);
            dx = xB - xA, dy = yB - yA, e = -type * dy, x = xA, y = yA;
            for (int i = 0; i <= dy; i++)
            {
                img.SetPixel(x, y, color);
                y++, e += 2 * dx;
                if (e * type >= 0)
                    x += type, e -= 2 * type * dy;
            }
        }

        printf("Draw a line from (%d, %d) to (%d, %d) using color (%f, %f, %f)\n", xA, yA, xB, yB, color.x(), color.y(), color.z());
    }
};

class Circle : public Element
{

public:
    int cx, cy;
    int radius;
    Vector3f color;
    void draw(Image &img) override
    {
        // TODO: Implement Algorithm to draw a Circle
        int x, y, d;
        x = 0, y = radius, d = 5 - 4 * radius;
        while (x <= y)
        {
            img.SetPixel(cx + x, cy + y, color), img.SetPixel(cx + y, cy + x, color);
            img.SetPixel(cx - x, cy + y, color), img.SetPixel(cx + y, cy - x, color);
            img.SetPixel(cx + x, cy - y, color), img.SetPixel(cx - y, cy + x, color);
            img.SetPixel(cx - x, cy - y, color), img.SetPixel(cx - y, cy - x, color);
            if (d < 0)
                d += 8 * x + 12;
            else
                d += 8 * (x - y) + 20, y--;
            x++;
        }
        printf("Draw a circle with center (%d, %d) and radius %d using color (%f, %f, %f)\n", cx, cy, radius,
               color.x(), color.y(), color.z());
    }
};

class Fill : public Element
{

public:
    int cx, cy;
    Vector3f color;
    void draw(Image &img) override
    {
        // TODO: Flood fill
        std::queue<int> xQ, yQ;
        Vector3f oldcolor = img.GetPixel(cx, cy);
        if (color != oldcolor)
        {
            int x, y, count = 0;
            xQ.push(cx), yQ.push(cy), img.SetPixel(cx, cy, color);
            while (!xQ.empty())
            {
                x = xQ.front(), y = yQ.front(), xQ.pop(), yQ.pop();
                if ((x + 1 < img.Width()) && (img.GetPixel(x + 1, y) == oldcolor))
                    xQ.push(x + 1), yQ.push(y), img.SetPixel(x + 1, y, color);
                if ((x > 0) && (img.GetPixel(x - 1, y) == oldcolor))
                    xQ.push(x - 1), yQ.push(y), img.SetPixel(x - 1, y, color);
                if ((y + 1 < img.Height()) && (img.GetPixel(x, y + 1) == oldcolor))
                    xQ.push(x), yQ.push(y + 1), img.SetPixel(x, y + 1, color);
                if ((y > 0) && (img.GetPixel(x, y - 1) == oldcolor))
                    xQ.push(x), yQ.push(y - 1), img.SetPixel(x, y - 1, color);
            }
        }
        printf("Flood fill source point = (%d, %d) using color (%f, %f, %f)\n", cx, cy,
               color.x(), color.y(), color.z());
    }
};