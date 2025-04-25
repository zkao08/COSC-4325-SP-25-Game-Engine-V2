// main.cpp
#define UNICODE
#include "InputHandler.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <Windows.h>
#include <vector>

#include <iostream> 

using Microsoft::WRL::ComPtr;

// Input colors
struct Color {
    float r, g, b, a;
};

const Color GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
const Color GRAY = { 0.5f, 0.5f, 0.5f, 1.0f };

// Globals
HWND g_hwnd = nullptr;
ComPtr<ID3D11Device> g_device;
ComPtr<ID3D11DeviceContext> g_context;
ComPtr<IDXGISwapChain> g_swapChain;
ComPtr<ID3D11RenderTargetView> g_renderTargetView;
bool g_running = true;

// Rectangle Vertex Structure
struct Vertex {
    float x, y;
    Color color;
};

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY || (msg == WM_KEYDOWN && wParam == VK_ESCAPE)) {
        PostQuitMessage(0);
        g_running = false;
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool InitWindow(HINSTANCE hInstance, int width, int height) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DXInputTestClass";
    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(0, wc.lpszClassName, L"DX Input Monitor",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, nullptr, nullptr, hInstance, nullptr);

    return g_hwnd != nullptr;
}

bool InitD3D(int width, int height) {
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = g_hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION, &scd, &g_swapChain,
        &g_device, nullptr, &g_context);
    if (FAILED(hr)) return false;

    ComPtr<ID3D11Texture2D> backBuffer;
    g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
    g_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &g_renderTargetView);
    g_context->OMSetRenderTargets(1, g_renderTargetView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT vp = {};
    vp.Width = (float)width;
    vp.Height = (float)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    g_context->RSSetViewports(1, &vp);
    return true;
}

void ClearScreen(float r, float g, float b) {
    float color[4] = { r, g, b, 1.0f };
    g_context->ClearRenderTargetView(g_renderTargetView.Get(), color);
}

void Present() {
    g_swapChain->Present(1, 0);
}

void DrawRect(float x, float y, float w, float h, const Color& color) {
    // Placeholder: normally you'd draw a triangle strip or setup a full shader pipeline
    // This function is just a stub — proper GPU-side rendering should be implemented
    // using a vertex buffer and shaders.
}

void Render(InputHandler& inputHandler) {
    ClearScreen(0.95f, 0.95f, 0.95f);

    const auto& keyboardMap = inputHandler.GetKeyboardMap();
    bool anyKey = false;

    for (const auto& [key, state] : keyboardMap) {
        if (state.isPressed) {
            std::cout << "Key Pressed: " << state.name << std::endl;
            anyKey = true;
        }
    }

    if (inputHandler.IsGamepadAvailable()) {
        const auto& gamepadMap = inputHandler.GetGamepadMap();
        for (const auto& [key, state] : gamepadMap) {
            if (state.isPressed) {
                std::cout << "Gamepad Button: " << state.name << std::endl;
            }
        }

        std::cout << "Left Stick: (" << inputHandler.GetGamepadAxisLeftX()
            << ", " << inputHandler.GetGamepadAxisLeftY() << ")\n";
    }

    if (!anyKey && !inputHandler.IsGamepadAvailable()) {
        //std::cout << "No input detected...\n";
    }

    Present();
}


int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    int width = 1600, height = 900;

    if (!InitWindow(hInstance, width, height)) return -1;
    ShowWindow(g_hwnd, SW_SHOW);
    if (!InitD3D(width, height)) return -1;

    InputHandler inputHandler;
    if (!inputHandler.Initialize()) return -1;

    MSG msg = {};
    // In main.cpp, modify the message loop
    while (g_running) {
        // Process Windows messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        inputHandler.Update();

        // Test if 'A' is pressed using our interface method
        if (inputHandler.IsKeyPressed(65)) { // ASCII for 'A'
            std::cout << "A key pressed\n";
        }

        // Compare with Windows input detection
        if (GetAsyncKeyState('A') & 0x8000) {
            std::cout << "Windows reports A key pressed\n";
        }

        Render(inputHandler);
    }

    inputHandler.Shutdown();
    return 0;
}
