// GameApp.h
#pragma once

#include <windows.h>
#include <memory>

#include "App/stdafx.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"

class Scene;

class GameApp
{
public:
    bool Initialize(HINSTANCE hInstance, int nCmdShow, WNDPROC wndProc);
    void Update();
    void Shutdown();
    void OnMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hWnd = nullptr;
    HINSTANCE m_hInstance = nullptr;

    UINT m_clientWidth = 1280;
    UINT m_clientHeight = 720;

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
};