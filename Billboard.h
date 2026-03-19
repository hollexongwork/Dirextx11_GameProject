#pragma once
#include "GameObject.h"

class Billboard : public GameObject
{
protected:
	class Camera* m_Camera;

	ID3D11Buffer* m_VertexBuffer;

	void MatrixSetting();
public:
	void Init()override;
	void Start()override;
	void Uninit()override;


};

