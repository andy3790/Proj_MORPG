// Camera.h
#pragma once
#include "App/stdafx.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

struct CameraData {
    XMMATRIX view;
    XMMATRIX projection;
};

class Camera {
public:
    void Initialize(ID3D12Device* device);
    void Update();
    void ApplyViewportsAndScissorRects(ID3D12GraphicsCommandList* commandList);

public: // getter
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_cbvHeap.Get(); }
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return m_cbvHeap->GetGPUDescriptorHandleForHeapStart(); }

public: // setter
    void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ);
    void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

private:
    CameraData m_data;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
    CameraData* m_mappedData = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap;

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
};
