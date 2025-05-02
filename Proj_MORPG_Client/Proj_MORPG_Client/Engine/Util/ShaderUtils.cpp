#include "Engine/Util/ShaderUtils.h"
#include <fstream>
#include <filesystem>

Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(
    LPCWSTR hlslPath,
    LPCSTR entryPoint,
    LPCSTR target,
    LPCWSTR csoPath)
{
    Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    bool useCSO = false;
    if (csoPath && std::filesystem::exists(csoPath))
    {
        if (SUCCEEDED(D3DReadFileToBlob(csoPath, &shaderBlob)))
        {
            OutputDebugString(L"Loaded shader from CSO file.\n");
            return shaderBlob;
        }
    }

    HRESULT hr = D3DCompileFromFile(
        hlslPath,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint,
        target,
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &shaderBlob,
        &errorBlob
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        throw std::runtime_error("Shader compilation failed.");
    }

    // 저장할 경로가 있고 컴파일에 성공했다면 저장
    if (csoPath)
    {
        std::ofstream ofs(csoPath, std::ios::binary);
        ofs.write(reinterpret_cast<const char*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize());
        ofs.close();
        OutputDebugString(L"Shader compiled and saved to CSO.\n");
    }

    return shaderBlob;
}
