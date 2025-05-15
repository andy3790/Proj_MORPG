#include "Engine/GameObject/GameObject.h"

using namespace DirectX;

void GameObject::Initialize(ID3D12Device* device, std::shared_ptr<Mesh> mesh)
{
    m_mesh = mesh;

    UINT cbSize = (sizeof(ObjectData) + 255) & ~255;

    // CBV Heap
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvHeap));

    // Constant Buffer
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);

    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_cbResource));

    // Map
    CD3DX12_RANGE range(0, 0);
    m_cbResource->Map(0, &range, reinterpret_cast<void**>(&m_mappedCB));

    // CBV
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_cbResource->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = cbSize;
    m_cbvGpuHandle = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();

    device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void GameObject::SetPosition(float x, float y, float z)
{
    m_position = { x, y, z };
}

void GameObject::RotateY(float angleRadians)
{
    m_rotationY += angleRadians;
}

void GameObject::Update()
{
    XMMATRIX trans = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    XMMATRIX rot = XMMatrixRotationY(m_rotationY);
    m_world = rot * trans;

    m_mappedCB->world = XMMatrixTranspose(m_world);
}

void GameObject::Render(ID3D12GraphicsCommandList* cmdList)
{
    ID3D12DescriptorHeap* heaps[] = { m_cbvHeap.Get() };
    cmdList->SetDescriptorHeaps(1, heaps);
    cmdList->SetGraphicsRootDescriptorTable(1, m_cbvGpuHandle); // b1

    m_mesh->Render(cmdList);
}
