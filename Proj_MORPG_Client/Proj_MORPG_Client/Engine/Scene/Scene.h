#pragma once
#include "App/stdafx.h"

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    void Initialize();
    void Update(float deltaTime);
    void Render(ID3D12GraphicsCommandList* commandList);
};