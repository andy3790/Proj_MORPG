// Camera.cpp
#include "Engine/Camera/Camera.h"

void Camera::Initialize(ID3D12Device* device)
{
    UINT bufferSize = (sizeof(CameraData) + 255) & ~255;

    // ConstantBuffer 생성
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer)
    );

    // Map
    CD3DX12_RANGE range(0, 0);
    m_constantBuffer->Map(0, &range, reinterpret_cast<void**>(&m_mappedData));

    // DescriptorHeap 생성
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvHeap));

    // CBV 생성
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = bufferSize;
    device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void Camera::Update()
{
    CameraData cam;
    cam.view = XMMatrixTranspose(XMMatrixIdentity());       // 추후 View 행렬로 교체
    cam.projection = XMMatrixTranspose(XMMatrixIdentity()); // 추후 Projection 행렬로 교체
    *m_mappedData = cam;
}