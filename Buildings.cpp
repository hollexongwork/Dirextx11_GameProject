#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "Buildings.h"
#include "Building.h"

void Buildings::Init()
{
	gen.seed(13456);

	m_Dist = std::uniform_real_distribution<float>(-20.0f, 20.0f);
	m_ModelDist = std::uniform_int_distribution<int>(0, 4);

	for (int z = 0; z < GRID_SIZE; ++z)
	{
		for (int x = 0; x < GRID_SIZE; ++x)
		{
			Building* building = Manager::GetScene()->AddGameObject<Building>(RENDER_LAYER_OPAQUE);
			building->SetModelIndex(m_ModelDist(gen));

			float offsetX = m_Dist(gen);
			float offsetZ = m_Dist(gen);

			float posX = m_StartPos.x + x * SPACING + offsetX;
			float posZ = m_StartPos.z + z * SPACING + offsetZ;

			building->SetPosition(Vector3(posX, 0.0f, posZ));
		}
	}
}
