#pragma once
#include<vector>
#include <random>

class PerlinNoise
{
private:
    std::vector<int> p; // Permutation table

    // Linear interpolation
    double lerp(double a, double b, double t) 
    {
        return a + t * (b - a);
    }

    // Fade function for smooth interpolation
    double fade(double t) 
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    // Gradient function
    double grad(int hash, double x, double y) 
    {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }
public:
    PerlinNoise() 
    {
        p.resize(256);
        for (int i = 0; i < 256; ++i) p[i] = i;

        std::mt19937 gen(12345);
        std::shuffle(p.begin(), p.end(), gen);
        p.insert(p.end(), p.begin(), p.end()); // Duplicate for overflow handling
    }

    double noise2D(double x, double y) 
    {
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;
        x -= std::floor(x);
        y -= std::floor(y);
        double u = fade(x);
        double v = fade(y);

        int aa = p[p[X] + Y];
        int ab = p[p[X] + Y + 1];
        int ba = p[p[X + 1] + Y];
        int bb = p[p[X + 1] + Y + 1];

        double x1 = lerp(grad(aa, x, y), grad(ba, x - 1, y), u);
        double x2 = lerp(grad(ab, x, y - 1), grad(bb, x - 1, y - 1), u);
        return (lerp(x1, x2, v) + 1) / 2;
    }
};

