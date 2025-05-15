// Camera.cpp
#include "Engine/Camera/Camera.h"

void Camera::Initialize(ID3D12Device* device)
{
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
    {
        m_viewport = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
        m_scissorRect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
    }
    {
        XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);     // 카메라 위치
        XMVECTOR target = XMVectorZero();                        // 바라보는 위치
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);        // 상단 방향

        m_data.view = XMMatrixLookAtLH(eye, target, up);

        float aspect = static_cast<float>(FRAME_BUFFER_WIDTH) / static_cast<float>(FRAME_BUFFER_HEIGHT);
        m_data.projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.1f, 100.0f);

        *m_mappedData = m_data;
    }
}

void Camera::Update()
{
    XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);     // 카메라 위치
    XMVECTOR target = XMVectorZero();                        // 바라보는 위치
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);        // 상단 방향

    m_data.view = XMMatrixLookAtLH(eye, target, up);

    *m_mappedData = m_data;
}

void Camera::ApplyViewportsAndScissorRects(ID3D12GraphicsCommandList* commandList)
{
    commandList->RSSetViewports(1, &m_viewport);
    commandList->RSSetScissorRects(1, &m_scissorRect);
}

void Camera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
    m_viewport.TopLeftX = float(xTopLeft);
    m_viewport.TopLeftY = float(yTopLeft);
    m_viewport.Width = float(nWidth);
    m_viewport.Height = float(nHeight);
    m_viewport.MinDepth = fMinZ;
    m_viewport.MaxDepth = fMaxZ;
}

void Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
    m_scissorRect.left = xLeft;
    m_scissorRect.top = yTop;
    m_scissorRect.right = xRight;
    m_scissorRect.bottom = yBottom;
}