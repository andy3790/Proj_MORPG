#include "Engine/Scene/Scene.h"
#include <iostream>

void Scene::Initialize()
{
    // TODO: �ε� �Ǵ� �ʱ�ȭ �ڵ�
    std::cout << "Scene initialized.\\n";
}

void Scene::Update(float deltaTime)
{
    // TODO: ���� ����, ������Ʈ �̵� ��
}

void Scene::Render(ID3D12GraphicsCommandList* commandList)
{
    // TODO: ������Ʈ �׸���
}