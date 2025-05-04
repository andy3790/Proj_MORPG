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

public:
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_cbvHeap.Get(); }
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return m_cbvHeap->GetGPUDescriptorHandleForHeapStart(); }

private:
    CameraData m_data;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
    CameraData* m_mappedData = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
};
