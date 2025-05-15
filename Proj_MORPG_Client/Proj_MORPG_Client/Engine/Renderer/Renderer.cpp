#include "Engine/Renderer/Renderer.h"
#include "Engine/RenderTypes/Vertex.h"
#include "Engine/Util/ShaderUtils.h"
#include <stdexcept>

Renderer::Renderer()
{
    m_nWndClientWidth = FRAME_BUFFER_WIDTH;
    m_nWndClientHeight = FRAME_BUFFER_HEIGHT;
}

bool Renderer::Initialize(HWND hWnd, UINT width, UINT height)
{
    if (!CreateDevice()) return false;
    CreateFenceAndSyncObjects();
    if (!CreateCommandObjects()) return false;
    if (!CreateSwapChain(hWnd)) return false;
    if (!CreateRenderTargetViews()) return false;

    CreateRootSignature();
    CreatePipelineState();

    CreateTriangleResources();

    return true;
}

void Renderer::BeginFrame()
{
    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), nullptr);

    // 작업 전 루트 시그니쳐 바인딩
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    // CBV 바인딩
    ID3D12DescriptorHeap* heaps[] = { m_camera->GetDescriptorHeap() };
    m_commandList->SetDescriptorHeaps(1, heaps);

    // b0 슬롯에 바인딩
    m_commandList->SetGraphicsRootDescriptorTable(0, m_camera->GetGpuHandle());

    // 리소스 상태 전환: Present -> RenderTarget
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_renderTargets[m_frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);

    // RTV 설정 및 Clear
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    const float clearColor[] = { 0.1f, 0.2f, 0.3f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // ViewPort, ScissorRect 적용 (Camera 관리)
    m_camera->ApplyViewportsAndScissorRects(m_commandList.Get());
}

void Renderer::EndFrame()
{
// RenderTarget -> Present 상태 전환
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_renderTargets[m_frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);

    // 커맨드 리스트 종료 및 제출
    m_commandList->Close();
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(1, cmdLists);

    // Present
    m_swapChain->Present(1, 0);

    // 다음 백버퍼 인덱스 획득
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Fence 대기
    WaitForGpuComplete();
}

void Renderer::Shutdown()
{
    WaitForGpuComplete();

    for (UINT i = 0; i < m_nSwapChainBuffers; ++i)
        m_renderTargets[i].Reset();

    m_rtvHeap.Reset();
    m_commandList.Reset();
    m_commandAllocator.Reset();
    m_commandQueue.Reset();
    m_swapChain.Reset();
    m_device.Reset();
    m_factory.Reset();

    CloseHandle(m_fenceEvent);
}

void Renderer::WaitForGpuComplete()
{
    const UINT64 currentFence = m_fenceValue;
    m_commandQueue->Signal(m_fence.Get(), currentFence);
    m_fenceValue++;

    if (m_fence->GetCompletedValue() < currentFence)
    {
        m_fence->SetEventOnCompletion(currentFence, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}


bool Renderer::CreateDevice()
{
    UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        debugController->EnableDebugLayer();
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)))) return false;

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    for (UINT i = 0; m_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device))))
            return true;
    }
    return false;
}

void Renderer::CreateFenceAndSyncObjects()
{
    m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    m_fenceValue = 1;
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

bool Renderer::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    if (FAILED(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)))) return false;
    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)))) return false;
    if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)))) return false;
    return true;
}

bool Renderer::CreateSwapChain(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = m_nSwapChainBuffers; // 일반적으로 2
    swapChainDesc.Width = m_nWndClientWidth;  // 혹은 동적으로 설정한 width
    swapChainDesc.Height = m_nWndClientHeight;  // 혹은 height
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 표준 색상 포맷
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1; // 멀티샘플링 비활성화

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    if (FAILED(m_factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),
        hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1)))
    {
        return false;
    }

    // IDXGISwapChain4로 업캐스팅
    if (FAILED(swapChain1.As(&m_swapChain)))
    {
        return false;
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    return true;
}

bool Renderer::CreateRenderTargetViews()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = m_nSwapChainBuffers;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap))))
        return false;

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (UINT i = 0; i < m_nSwapChainBuffers; ++i)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]))))
            return false;

        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, m_rtvDescriptorSize);
    }

    return true;
}

void Renderer::CreateRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE cbvRange;
    cbvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // register(b0) Camera

    CD3DX12_ROOT_PARAMETER rootParam;
    rootParam.InitAsDescriptorTable(1, &cbvRange);

    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 1;
    rootSigDesc.pParameters = &rootParam;
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

    HRESULT hr = D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig,
        &errorBlob
    );

    if (FAILED(hr))
    {
        if (errorBlob)
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        throw std::runtime_error("Failed to serialize root signature.");
    }

    m_device->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)
    );
}

void Renderer::CreatePipelineState()
{
    // 정점 구조 입력 레이아웃 정의
    D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
          0, D3D12_APPEND_ALIGNED_ELEMENT,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
          0, D3D12_APPEND_ALIGNED_ELEMENT,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // 셰이더 로드 (.cso가 있으면 로드, 없으면 컴파일)
    auto vsBlob = LoadShader(
        L"Shaders\\Triangle.hlsl", "VSMain", "vs_5_1", L"Shaders\\TriangleVS.cso");

    auto psBlob = LoadShader(
        L"Shaders\\Triangle.hlsl", "PSMain", "ps_5_1", L"Shaders\\TrianglePS.cso");

    // PSO 디스크립터 설정
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    HRESULT hr = m_device->CreateGraphicsPipelineState(
        &psoDesc, IID_PPV_ARGS(&m_pipelineState));

    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create PSO");
    }
}

void Renderer::CreateTriangleResources()
{
    Vertex triangleVertices[] = {
       { { 0.0f, 0.25f, 0.0f }, { 1, 0, 0, 1 } },
       { { 0.25f, -0.25f, 0.0f }, { 0, 1, 0, 1 } },
       { { -0.25f, -0.25f, 0.0f }, { 0, 0, 1, 1 } }
    };

    const UINT vbSize = sizeof(triangleVertices);

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);

    m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer));

    void* mappedData = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    m_vertexBuffer->Map(0, &readRange, &mappedData);
    memcpy(mappedData, triangleVertices, vbSize);
    m_vertexBuffer->Unmap(0, nullptr);

    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.SizeInBytes = vbSize;
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
}
