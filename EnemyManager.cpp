#include "main.h"
#include "Manager.h"
#include "Scene.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include <random>

int EnemyManager::m_AliveCount = 0;

void EnemyManager::Init()
{
	m_AliveCount = 0;
}

void EnemyManager::CreateEnemyRandomRotation(const Vector3& pos)
{
    static std::mt19937 engine{ std::random_device{}() };
    static std::uniform_real_distribution<float> dist(0.0f, 360.0f);

    Enemy* enemy = Manager::GetScene()->AddGameObject<Enemy>(RENDER_LAYER_OPAQUE);
    enemy->SetPosition(pos);

    float angleY = dist(engine);

    enemy->SetRotation({ 0.0f, angleY, 0.0f });
    enemy->UpdateRotationMatrix();


}