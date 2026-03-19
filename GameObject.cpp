#include "GameObject.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

void GameObject::UpdateColliderScales(Vector3 targetScale)
{
    Vector3 scaleRatio = targetScale / Scale;

    if (Scale.x == 0.0f) scaleRatio.x = 1.0f;
    if (Scale.y == 0.0f) scaleRatio.y = 1.0f;
    if (Scale.z == 0.0f) scaleRatio.z = 1.0f;


    for (auto& comp : Components) 
    {
        if (auto* box = dynamic_cast<BoxCollider*>(comp.get())) 
        {
            Vector3 half = box->GetHalfSize();
            box->SetHalfSize(Vector3(half.x * scaleRatio.x, half.y * scaleRatio.y, half.z * scaleRatio.z));
        }
        else if (auto* sphere = dynamic_cast<SphereCollider*>(comp.get())) 
        {
            float maxScale = fmax(fmax(scaleRatio.x, scaleRatio.y), scaleRatio.z);
            sphere->SetRadius(sphere->GetRadius() * maxScale);
        }
    }
}
