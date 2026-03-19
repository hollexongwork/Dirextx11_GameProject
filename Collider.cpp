#include "Collider.h"
#include "Collision.h" 

Collider::Collider()
{
	Collision::Register(this);
}

Collider::~Collider()
{
	Collision::Unregister(this);
}

Vector3 Collider::GetCurrentPosition() const
{
	if (m_GameObject)
	{
		Vector3 pos = m_GameObject->GetPosition();
		pos += m_GameObject->GetAxisX() * m_Offset.x;
		pos += m_GameObject->GetAxisY() * m_Offset.y;
		pos += m_GameObject->GetAxisZ() * m_Offset.z;
		return pos;
	}
	else
	{
		return Vector3(0, 0, 0);
	}
}
