// GameApp.cpp
#include "App/GameApp.h"

bool GameApp::Initialize(HINSTANCE hInstance, int nCmdShow, WNDPROC wndProc)
{
    m_hInstance = hInstance;

    const wchar_t CLASS_NAME[] = L"MORPGClientWindowClass";
    const wchar_t WINDOW_TITLE[] = L"MORPG Client";

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassEx(&wc))
        return false;

    RECT windowRect = { 0, 0, static_cast<LONG>(FRAME_BUFFER_WIDTH), static_cast<LONG>(FRAME_BUFFER_HEIGHT) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWnd = CreateWindow(
        CLASS_NAME, WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr,
        hInstance, nullptr);

    if (!m_hWnd)
        return false;

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

    // ������ ���� �� �ʱ�ȭ
    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->Initialize(m_hWnd, m_clientWidth, m_clientHeight))
        return false;

    return true;
}

void GameApp::Update()
{
    m_renderer->BeginFrame();
    // TODO: ��, UI �� ������
    m_renderer->EndFrame();
}

void GameApp::Shutdown()
{
    if (m_renderer) m_renderer->Shutdown();
}

void GameApp::OnMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // ����: �Է� ó��, ������ �޽��� �ݿ�
}
