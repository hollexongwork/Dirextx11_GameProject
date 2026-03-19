#include "Collision.h"
#include <unordered_map>
#include <unordered_set>
#include "Bullet.h"
std::vector<Collider*> Collision::s_Colliders;

struct PairHash
{
    size_t operator()(const std::pair<Collider*, Collider*>& p) const
    {
        return std::hash<Collider*>()(p.first) ^
            (std::hash<Collider*>()(p.second) << 1);
    }
};

void Collision::Update()
{
    std::unordered_set<std::pair<Collider*, Collider*>, PairHash> finalContacts;

    for (int iteration = 0; iteration < MAX_SOLVE_ITERATIONS; ++iteration)
    {
        bool anyCollision = false;
        std::unordered_map<Collider*, CollisionInfo> bestHits;

        // Collision Check
        for (size_t i = 0; i < s_Colliders.size(); ++i)
        {
            for (size_t j = i + 1; j < s_Colliders.size(); ++j)
            {
                Collider* a = s_Colliders[i];
                Collider* b = s_Colliders[j];

                if (!a || !b) continue;

                CollisionInfo infoA;
                if (a->Intersects(b, infoA))
                {
                    infoA.Self = a;
                    infoA.Other = b;

                    if (!bestHits.count(a) || infoA.T < bestHits[a].T)
                        bestHits[a] = infoA;

                    finalContacts.insert({ a, b });
                }

                CollisionInfo infoB;
                if (b->Intersects(a, infoB))
                {
                    infoB.Self = b;
                    infoB.Other = a;

                    if (!bestHits.count(b) || infoB.T < bestHits[b].T)
                        bestHits[b] = infoB;

                    finalContacts.insert({ b, a });
                }
            }
        }

        if (bestHits.empty())
            break;


        // Fix Position
        for (auto& [collider, info] : bestHits)
        {
            Collider* self = collider;
            Collider* other = info.Other;

            bool selfTrigger = self->IsTrigger();
            bool otherTrigger = other->IsTrigger();

            bool shouldResolve = false;

            // --- Trigger Check ---
            if (selfTrigger && !otherTrigger)
            {
                shouldResolve = true;
            }
            else if (!selfTrigger && otherTrigger)
            {
                shouldResolve = false;
            }
            else
            {
                shouldResolve = !self->IsStatic();
            }

            if (shouldResolve)
            {
                self->ApplyResolvedPosition(info.ResolvedPosition);

                if (Bullet* bullet = self->GetOwnerAs<Bullet>())
                {
                    Vector3 v = bullet->GetVelocity();
                    float vn = Vector3::Dot(v, info.Normal);

                    if (vn < 0)
                    {
                        v -= info.Normal * vn;
                        bullet->SetVelocity(v);
                    }
                }

                self->SavePreviousTransform();
                anyCollision = true;
            }
        }

        if (!anyCollision)
            break;
    }

    // Event
    for (auto& pair : finalContacts)
    {
        pair.first->OnCollisionEnter(pair.second);
    }
}

