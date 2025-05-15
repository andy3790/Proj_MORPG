// Mesh.h
#pragma once
#include "App/stdafx.h"

class Mesh
{
public:
    void Initialize(ID3D12Device* device);
    void Render(ID3D12GraphicsCommandList* commandList);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
};
