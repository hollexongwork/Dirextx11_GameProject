#include "FieldCollider.h"
#include "SphereCollider.h"
#include "MeshField.h"

void FieldCollider::Start()
{
    m_Field = GetOwnerAs<MeshField>();
}

bool FieldCollider::Intersects(const Collider* other, CollisionInfo& info) const
{
    if (!other)
        return false;

    if (auto* sphere = dynamic_cast<const SphereCollider*>(other))
    {
        HitResult hit;

        Vector3 start = sphere->GetPreviousPosition();
        Vector3 end = sphere->GetCurrentPosition();
        float   radius = sphere->GetRadius();

        if (MeshFieldVsSweptSphere(start, end, radius, hit))
        {
            info.Hit = true;
            info.Self = const_cast<FieldCollider*>(this);
            info.Other = const_cast<Collider*>(other);

            info.T = hit.T;
            info.Normal = hit.HitNormal;
            info.ResolvedPosition = hit.HitPoint;

            return true;
        }
    }
	return false;
}

static Vector3 ClosestPointOnTriangle(const Vector3& p, const Triangle& tri)
{
    const Vector3& a = tri.v0;
    const Vector3& b = tri.v1;
    const Vector3& c = tri.v2;

    Vector3 ab = b - a;
    Vector3 ac = c - a;
    Vector3 ap = p - a;

    float d1 = Vector3::Dot(ab, ap);
    float d2 = Vector3::Dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a;

    Vector3 bp = p - b;
    float d3 = Vector3::Dot(ab, bp);
    float d4 = Vector3::Dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return b;

    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
    {
        float v = d1 / (d1 - d3);
        return a + ab * v;
    }

    Vector3 cp = p - c;
    float d5 = Vector3::Dot(ab, cp);
    float d6 = Vector3::Dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c;

    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
    {
        float w = d2 / (d2 - d6);
        return a + ac * w;
    }

    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
    {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + (c - b) * w;
    }

    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + ab * v + ac * w;
}

bool FieldCollider::MeshFieldVsSweptSphere(const Vector3& start, const Vector3& end, const float& radius, HitResult& outHit) const
{
    Vector3 move = end - start;
    float moveLen = move.length();
    if (moveLen < 1e-6f) return false;

    Vector3 dir = move / moveLen;
    float minT = 1.0f + 1e-5f;
    Vector3 bestNormal(0, 1, 0);
    Vector3 bestCenter = end;

    for (const Triangle& tri : m_Field->GetTriangles())
    {
        float ndotd = Vector3::Dot(tri.normal, dir);
        if (fabs(ndotd) > 1e-6f)
        {
            float signedDist = Vector3::Dot(tri.normal, start - tri.v0);
            float approach = -signedDist;

            if (signedDist > -radius && ndotd < 0.0f)
            {
                float t_plane = (radius + signedDist) / -ndotd;
                if (t_plane >= 0.0f && t_plane <= moveLen)
                {
                    Vector3 centerAtT = start + dir * t_plane;
                    Vector3 closest = ClosestPointOnTriangle(centerAtT, tri);
                    Vector3 toCenter = centerAtT - closest;
                    float distSq = toCenter.lengthSq();

                    if (distSq <= radius * radius + 1e-6f)
                    {
                        float t_norm = t_plane / moveLen;
                        if (t_norm < minT)
                        {
                            minT = t_norm;
                            bestCenter = centerAtT;
                            if (distSq > 1e-8f)
                            {
								bestNormal = toCenter;
								bestNormal.normalize();
							}
                            else
                            {
                                bestNormal = tri.normal;
                            }
                        }
                    }
                }
            }
        }

        const Vector3 verts[3] = { tri.v0, tri.v1, tri.v2 };
        for (int i = 0; i < 3; ++i)
        {
            Vector3 a = verts[i];
            Vector3 b = verts[(i + 1) % 3];
            Vector3 edge = b - a;
            float edgeLen = edge.length();
            if (edgeLen < 1e-5f) continue;

            Vector3 unitEdge = edge / edgeLen;

            Vector3 w0 = start - a;
            float a_coeff = Vector3::Dot(dir, dir) - Vector3::Dot(unitEdge, unitEdge);
            if (fabs(a_coeff) < 1e-6f) continue;

            float b_coeff = 2.0f * (Vector3::Dot(dir, w0) - Vector3::Dot(unitEdge, w0));
            float c_coeff = Vector3::Dot(w0, w0) - radius * radius;

            float disc = b_coeff * b_coeff - 4.0f * a_coeff * c_coeff;
            if (disc < 0.0f) continue;

            float sqrtDisc = sqrtf(disc);
            float t1 = (-b_coeff - sqrtDisc) / (2.0f * a_coeff);
            float t2 = (-b_coeff + sqrtDisc) / (2.0f * a_coeff);

            for (float t : {t1, t2})
            {
                if (t < 0.0f || t > 1.0f) continue;

                Vector3 posOnMove = start + dir * (t * moveLen);
                float proj = Vector3::Dot(posOnMove - a, unitEdge);
                proj = Clamp(proj, 0.0f, edgeLen);

                Vector3 closestOnEdge = a + unitEdge * proj;
                Vector3 diff = posOnMove - closestOnEdge;
                float d2 = diff.lengthSq();

                if (d2 <= radius * radius + 1e-6f)
                {
                    if (t < minT)
                    {
                        minT = t;
                        bestCenter = posOnMove;

                        if (d2 > 1e-8f)
                        {
                            bestNormal = diff;
							bestNormal.normalize();
                        }
                        else
                        {
							bestNormal = tri.normal;
                        }
                    }
                }
            }
        }
    }

    if (minT >= 1.0f) return false;

    outHit.Hit = true;
    outHit.T = minT;
    outHit.HitNormal = bestNormal;
    outHit.HitPoint = start + move * minT - bestNormal * radius;

    return true;
}