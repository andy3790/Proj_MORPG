#pragma once
#include "App/stdafx.h"

Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(
    LPCWSTR hlslPath,
    LPCSTR entryPoint,
    LPCSTR target,
    LPCWSTR csoPath = nullptr  // ������ .cso ��� (���� ����)
);
