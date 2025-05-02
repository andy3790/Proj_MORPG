#pragma once
#include "App/stdafx.h"

class Renderer;

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    void Initialize(Renderer* renderer);
    void Update(float deltaTime);
    void Render(ID3D12GraphicsCommandList* commandList);

private:
    Renderer* m_renderer = nullptr;
};