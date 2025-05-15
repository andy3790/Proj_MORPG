// Mesh.cpp
#include "App/stdafx.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/RenderTypes/Vertex.h"

using namespace DirectX;

void Mesh::Initialize(ID3D12Device* device)
{
    std::vector<Vertex> vertices = {
       { { 0.0f, 0.25f, 0.0f }, { 1, 0, 0, 1 } },
       { { 0.25f, -0.25f, 0.0f }, { 0, 1, 0, 1 } },
       { { -0.25f, -0.25f, 0.0f }, { 0, 0, 1, 1 } }
    };

    UINT vbSize = static_cast<UINT>(sizeof(Vertex) * vertices.size());

    // 滚欺 积己
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);

    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer));

    // Map -> 汗荤
    void* pData = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    m_vertexBuffer->Map(0, &readRange, &pData);
    memcpy(pData, vertices.data(), vbSize);
    m_vertexBuffer->Unmap(0, nullptr);

    // View 积己
    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.SizeInBytes = vbSize;
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void Mesh::Render(ID3D12GraphicsCommandList* commandList)
{
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    commandList->DrawInstanced(3, 1, 0, 0);
}
