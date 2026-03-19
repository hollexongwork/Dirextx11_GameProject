#pragma once
#include <vector>
#include <algorithm>
#include "Collider.h"


class Collision
{
private:
    static constexpr int MAX_SOLVE_ITERATIONS = 2;

    static std::vector<Collider*> s_Colliders;
public:
    static void Register(Collider* col) { s_Colliders.push_back(col); }
    static void Unregister(Collider* col)
    {
        s_Colliders.erase(std::remove(s_Colliders.begin(), s_Colliders.end(), col), s_Colliders.end());
    }

    static void Clear() { s_Colliders.clear(); }

    static void Update();

    
};