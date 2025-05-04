#pragma once
#include "App/stdafx.h"
#include "Engine/Camera/Camera.h"

class Renderer;

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

public:
    void Initialize(Renderer* renderer);
    void Update(float deltaTime);
    void Render(ID3D12GraphicsCommandList* commandList);

public:
    Camera* GetCamera() const { return m_camera.get(); }

private:
    Renderer* m_renderer = nullptr;
    std::unique_ptr<Camera> m_camera;
};