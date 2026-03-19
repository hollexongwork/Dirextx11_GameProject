#include "BoxCollider.h"
#include "SphereCollider.h"

void BoxCollider::Start()
{
    m_Owner = GetOwner();
}

bool BoxCollider::Intersects(const Collider* other, CollisionInfo& info) const
{
    if (const auto* box = dynamic_cast<const BoxCollider*>(other))
    {
        MTV mtv;
        if (!ObbVsObb(box, mtv))
            return false;

        info.Hit = true;
        info.Normal = mtv.axis;

        Vector3 corrected = GetOwner()->GetPosition() - mtv.axis * mtv.depth;

        info.ResolvedPosition = corrected;
        info.T = 0.0f;

        return true;
    }

    if (dynamic_cast<const SphereCollider*>(other))
    {
        return other->Intersects(this, info);
    }

    return false;
}

void BoxCollider::ApplyResolvedPosition(const Vector3& pos)
{
    if (IsStatic()) return;

    m_Owner->SetPosition(pos);
}

bool BoxCollider::TestAxis(const Vector3& axis, float radiusA, float radiusB, float signedDist, MTV& mtv) const
{
    float absDist = fabsf(signedDist);
    float overlap = radiusA + radiusB - absDist;

    if (overlap < 0.0f)
    {
        return false;
    }

    if (overlap < mtv.depth) 
    {
        mtv.depth = overlap;
        mtv.axis = (signedDist < 0.0f) ? -axis : axis;
    }

    return true;
}

bool BoxCollider::ObbVsObb(const BoxCollider* other, MTV& mtv) const
{
    GameObject* objA = GetOwner();
    GameObject* objB = other->GetOwner();

    Vector3 centerA = GetCurrentPosition();
    Vector3 centerB = other->GetCurrentPosition();

    Vector3 halfA = m_HalfSize;
    Vector3 halfB = other->m_HalfSize;

    Vector3 A[3] = { objA->GetAxisX(), objA->GetAxisY(), objA->GetAxisZ() };
    Vector3 B[3] = { objB->GetAxisX(), objB->GetAxisY(), objB->GetAxisZ() };

    Vector3 T = centerB - centerA;

    const float epsilon = 1e-6f;

    float R[3][3], AbsR[3][3];

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            R[i][j] = Vector3::Dot(A[i], B[j]);
            AbsR[i][j] = fabsf(R[i][j]) + epsilon;
        }
    }

    // --- Local A Axis ---
    for (int i = 0; i < 3; i++)
    {
        float radiusA = halfA[i];
        float radiusB = halfB.x * AbsR[i][0] + halfB.y * AbsR[i][1] + halfB.z * AbsR[i][2];
        float signedDist = Vector3::Dot(T, A[i]);

        if (!TestAxis(A[i], radiusA, radiusB, signedDist, mtv))
        {
            return false;
        }
    }

    // --- Local B Axis ---
    for (int i = 0; i < 3; i++)
    {
        float radiusA = halfA.x * AbsR[0][i] + halfA.y * AbsR[1][i] + halfA.z * AbsR[2][i];
        float radiusB = halfB[i];
        float signedDist = Vector3::Dot(T, B[i]);

        if (!TestAxis(B[i], radiusA, radiusB, signedDist, mtv))
        {
            return false;
        }
    }

    // --- Dot Axis ---
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Vector3 cross = Vector3::Cross(A[i], B[j]);
            float lenSq = cross.lengthSq();
            if (lenSq < epsilon)
            {
                continue;
            }

            Vector3 axis = cross / sqrtf(lenSq);

            float radiusA = halfA[(i + 1) % 3] * AbsR[(i + 2) % 3][j] +
                            halfA[(i + 2) % 3] * AbsR[(i + 1) % 3][j];

            float radiusB = halfB[(j + 1) % 3] * AbsR[i][(j + 2) % 3] +
                            halfB[(j + 2) % 3] * AbsR[i][(j + 1) % 3];

            float signedDist = Vector3::Dot(T, A[(i + 1) % 3]) * R[(i + 2) % 3][j] -
                               Vector3::Dot(T, A[(i + 2) % 3]) * R[(i + 1) % 3][j];

            float absDist = fabsf(signedDist);
            float overlap = radiusA + radiusB - absDist;

            float dist = Vector3::Dot(T, axis);

            if (overlap < 0.0f)
            {
                return false;
            }

            if (overlap < mtv.depth)
            {
                mtv.depth = overlap;
                mtv.axis = (dist < 0.0f) ? -axis : axis;
            }
        }
    }

    return true;
}


