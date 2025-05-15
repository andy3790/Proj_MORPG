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

    m_mesh = std::make_unique<Mesh>();
    m_mesh->Initialize(renderer->GetDevice());


    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    mesh->Initialize(renderer->GetDevice());

    m_obj = std::make_unique<GameObject>();
    m_obj->Initialize(renderer->GetDevice(), mesh);
    m_obj->SetPosition(0, 0, 0);
}

void Scene::Update(float deltaTime)
{
    // TODO: 게임 로직, 오브젝트 이동 등
    m_obj->Update();
}

void Scene::Render(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(m_renderer->GetPipelineState());
    commandList->SetGraphicsRootSignature(m_renderer->GetRootSignature());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //m_mesh->Render(commandList);
    m_obj->Render(commandList);

    //D3D12_VERTEX_BUFFER_VIEW* vbView = m_renderer->GetVertexBufferView();
    //commandList->IASetVertexBuffers(0, 1, vbView);

    //commandList->DrawInstanced(3, 1, 0, 0); // 삼각형 1개 (정점 3개)
}