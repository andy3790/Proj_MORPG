#pragma once
#include "App/stdafx.h"
#include "Engine/Mesh/Mesh.h"

class GameObject
{
public:
    void Initialize(ID3D12Device* device, std::shared_ptr<Mesh> mesh);
    void SetPosition(float x, float y, float z);
    void RotateY(float angleRadians);

    void Update();
    void Render(ID3D12GraphicsCommandList* cmdList);

private:
    std::shared_ptr<Mesh> m_mesh;

    DirectX::XMFLOAT3 m_position = { 0, 0, 0 };
    float m_rotationY = 0.0f;

    DirectX::XMMATRIX m_world = DirectX::XMMatrixIdentity();

    struct ObjectData {
        DirectX::XMMATRIX world;
    };

    Microsoft::WRL::ComPtr<ID3D12Resource> m_cbResource;
    ObjectData* m_mappedCB = nullptr;

    D3D12_GPU_DESCRIPTOR_HANDLE m_cbvGpuHandle = {};
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
};
