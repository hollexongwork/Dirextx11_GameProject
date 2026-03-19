#pragma once

class EnemyManager
{
private:
	static int m_AliveCount;

public:
    static void Init();

    static void RegisterEnemy() { m_AliveCount++; }
    static void UnregisterEnemy() { m_AliveCount--; }

    static bool IsAllDestory() { return m_AliveCount <= 0; }

    static void CreateEnemyRandomRotation(const Vector3& pos);
};

