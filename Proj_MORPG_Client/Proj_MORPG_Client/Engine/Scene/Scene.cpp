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


    // 공유 메쉬 생성 (삼각형)
    m_sharedMesh = std::make_shared<Mesh>();
    m_sharedMesh->Initialize(m_renderer->GetDevice());

    // GameObject 여러 개 생성
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
        obj->RotateY(deltaTime); // 간단한 회전
        obj->Update();
    }
}

void Scene::Render(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(m_renderer->GetPipelineState());
    commandList->SetGraphicsRootSignature(m_renderer->GetRootSignature());

    //// 공통 힙 (카메라 + 오브젝트 CBV가 들어있는 Heap들)
    //ID3D12DescriptorHeap* heaps[] = {
    //    m_renderer->GetCameraDescriptorHeap(), // b0 heap
    //    m_objects[0]->GetCbvHeap()             // b1 heap (모두 같은 구조라면 하나만 대표로 사용 가능)
    //};
    //cmdList->SetDescriptorHeaps(1, heaps); // 한번만 호출

    //// b0: 카메라 CBV 바인딩
    //cmdList->SetGraphicsRootDescriptorTable(0, m_renderer->GetCameraGpuHandle());


    for (auto& obj : m_objects)
    {
        obj->Render(commandList);
    }
}