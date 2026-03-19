#pragma once
#include "Loading.h"

class LoadTitle : public Loading
{
public:
	void Init() override;
	void Start() override;
	void Uninit() override;
	void Update() override;
};

