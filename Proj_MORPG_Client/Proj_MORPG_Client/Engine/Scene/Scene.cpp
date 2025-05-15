#include "Engine/Scene/Scene.h"
#include "App/stdafx.h"
#include "Engine/Renderer/Renderer.h"
#include <memory>


void Scene::Initialize(Renderer* renderer)
{
    m_renderer = renderer;
    m_camera = std::make_unique<Camera>();
    m_camera->Initialize(m_renderer->GetDevice());

    m_renderer->SetCamera(m_camera.get());


    // ���� �޽� ���� (�ﰢ��)
    m_sharedMesh = std::make_shared<Mesh>();
    m_sharedMesh->Initialize(m_renderer->GetDevice());

    // GameObject ���� �� ����
    for (int i = 0; i < 5; ++i)
    {
        auto obj = std::make_unique<GameObject>();
        obj->Initialize(m_renderer->GetDevice(), m_sharedMesh);

        float x = (i - 2) * 0.6f;
        obj->SetPosition(x, 0.0f, 0.0f);

        m_objects.push_back(std::move(obj));
    }
}

void Scene::Update(float deltaTime)
{
    for (auto& obj : m_objects)
    {
        obj->RotateY(deltaTime); // ������ ȸ��
        obj->Update();
    }
}

void Scene::Render(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(m_renderer->GetPipelineState());
    commandList->SetGraphicsRootSignature(m_renderer->GetRootSignature());

    for (auto& obj : m_objects)
    {
        obj->Render(commandList);
    }
}