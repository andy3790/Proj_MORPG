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
    ID3D12GraphicsCommandList* GetCommandList() { return m_commandList.Get(); }

private:
    void WaitForGpuComplete();
    bool CreateDevice();
    bool CreateSwapChain(HWND hWnd);
    bool CreateCommandObjects();
    bool CreateRenderTargetViews();

private:
    Microsoft::WRL::ComPtr<IDXGIFactory6> m_factory;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

    static const UINT m_nSwapChainBuffers = 2;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[m_nSwapChainBuffers];
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    UINT m_rtvDescriptorSize = 0;
    UINT m_frameIndex = 0;

    int m_nWndClientWidth = 0;
    int m_nWndClientHeight = 0;
};
