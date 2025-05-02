#include "Engine/Scene/Scene.h"
#include <iostream>

void Scene::Initialize()
{
    // TODO: 로딩 또는 초기화 코드
    std::cout << "Scene initialized.\\n";
}

void Scene::Update(float deltaTime)
{
    // TODO: 게임 로직, 오브젝트 이동 등
}

void Scene::Render(ID3D12GraphicsCommandList* commandList)
{
    // TODO: 오브젝트 그리기
}