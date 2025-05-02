#pragma once

#include "App/stdafx.h"
#include <dxgi1_6.h>

class Renderer
{
public:
    Renderer();
    ~Renderer() {};

public:
    bool Initialize(HWND hWnd, UINT width, UINT height);
    void BeginFrame();
    void EndFrame();
    void Shutdown();

public: // Getter
    ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }
    ID3D12PipelineState* GetPipelineState() const { return m_pipelineState.Get(); }
    ID3D12RootSignature* GetRootSignature() const { return m_rootSignature.Get(); }
    D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &m_vertexBufferView; }


private:
    void WaitForGpuComplete();
    bool CreateDevice();
    void CreateFenceAndSyncObjects();
    bool CreateSwapChain(HWND hWnd);
    bool CreateCommandObjects();
    bool CreateRenderTargetViews();

private:
    void CreateRootSignature();
    void CreatePipelineState();
    void CreateTriangleResources();

private:
    Microsoft::WRL::ComPtr<IDXGIFactory6> m_factory;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue = 0;
    HANDLE m_fenceEvent = nullptr;

    static const UINT m_nSwapChainBuffers = 2;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[m_nSwapChainBuffers];
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    UINT m_rtvDescriptorSize = 0;
    UINT m_frameIndex = 0;

    int m_nWndClientWidth = 0;
    int m_nWndClientHeight = 0;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

    Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
};
