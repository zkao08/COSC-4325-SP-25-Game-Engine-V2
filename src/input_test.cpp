// main.cpp
// DirectX window test application for InputHandler class

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "InputHandler.h"
#include <iomanip>
#include <chrono>
#include <thread>

// Link the required DirectX libraries
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// Window and DirectX globals
HWND g_hwnd = NULL;
ID3D11Device* g_pDevice = nullptr;
ID3D11DeviceContext* g_pDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
InputHandler g_Input;
bool g_Running = true;

// Font information struct for rendering text
struct FontVertex
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT2 TexCoord;
};

// Function Prototypes
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT InitializeWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd);
HRESULT InitializeDirectX(HWND hWnd);
void Render();
void CleanupDirectX();
void RenderText(const std::string& text, float x, float y, float scale);

// Standard main function entry point
int main(int argc, char* argv[])
{
    // Get application instance
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Initialize the window
    if (FAILED(InitializeWindow(hInstance, SW_SHOW, g_hwnd)))
    {
        MessageBox(NULL, "Window Initialization Failed!", "Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Initialize DirectX
    if (FAILED(InitializeDirectX(g_hwnd)))
    {
        MessageBox(NULL, "DirectX Initialization Failed!", "Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Initialize input handler
    g_Input.startUp(g_hwnd);

    // Show the window
    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);

    // Main message loop
    MSG msg = {};

    // Setup timing
    LARGE_INTEGER frequency;
    LARGE_INTEGER lastTime;
    LARGE_INTEGER currentTime;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    // Main application loop
    while (g_Running)
    {
        // Process all Windows messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                g_Running = false;
                break;
            }
        }

        // Calculate delta time
        QueryPerformanceCounter(&currentTime);
        float deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / static_cast<float>(frequency.QuadPart);
        lastTime = currentTime;

        // Update input
        g_Input.Update();

        // Check for escape key to exit
        if (g_Input.IsKeyDown(KeyCode::ESCAPE))
        {
            g_Running = false;
        }

        // Render
        Render();
    }

    // Clean up
    CleanupDirectX();

    return static_cast<int>(msg.wParam);
}

// Initialize the window
HRESULT InitializeWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd)
{
    // Register window class
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = "DirectXInputTestWindowClass";

    if (!RegisterClassEx(&wcex))
    {
        return E_FAIL;
    }

    // Create window
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindow(
        "DirectXInputTestWindowClass",
        "DirectX Input Handler Test",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        return E_FAIL;
    }

    return S_OK;
}

// Initialize DirectX
HRESULT InitializeDirectX(HWND hWnd)
{
    HRESULT hr = S_OK;

    // Get window size
    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    // Create device and swap chain
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    D3D_FEATURE_LEVEL featureLevel;

    hr = D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        createDeviceFlags,
        featureLevels,
        numFeatureLevels,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pDevice,
        &featureLevel,
        &g_pDeviceContext
    );

    if (FAILED(hr))
    {
        return hr;
    }

    // Create render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
    {
        return hr;
    }

    hr = g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
    {
        return hr;
    }

    // Set render target
    g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

    // Set viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pDeviceContext->RSSetViewports(1, &vp);

    return S_OK;
}

// Render the scene
void Render()
{
    if (!g_pDeviceContext || !g_pRenderTargetView || !g_pSwapChain)
        return;

    // Clear the render target
    float clearColor[4] = { 0.0f, 0.1f, 0.2f, 1.0f }; // Dark blue background
    g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);

    // Get window dimensions
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    int windowWidth = rc.right - rc.left;
    int windowHeight = rc.bottom - rc.top;

    // In a real application, we would render text using DirectX
    // This would be done using sprites, textures, and shaders
    // For this example, we'll just present the swap chain

    // Present the scene
    g_pSwapChain->Present(1, 0);

    // Print input state to console (for this example)
    // In a real DirectX application, you would render this on screen
    std::cout << "\x1B[2J\x1B[H"; // Clear console

    std::cout << "DirectX Input Handler Test" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;
    std::cout << std::endl;

    // Display mouse state
    const MouseState& mouse = g_Input.GetMouseState();
    std::cout << "Mouse Position: (" << mouse.x << ", " << mouse.y << ")" << std::endl;
    std::cout << "Mouse Delta: (" << mouse.deltaX << ", " << mouse.deltaY << ")" << std::endl;
    std::cout << "Mouse Wheel: " << mouse.wheelDelta << std::endl;
    std::cout << "Mouse Buttons:" << std::endl;
    std::cout << "  Left: " << (mouse.buttons[MouseButton::LEFT] ? "Down" : "Up") << std::endl;
    std::cout << "  Right: " << (mouse.buttons[MouseButton::RIGHT] ? "Down" : "Up") << std::endl;
    std::cout << "  Middle: " << (mouse.buttons[MouseButton::MIDDLE] ? "Down" : "Up") << std::endl;
    std::cout << std::endl;

    // Display keyboard state - show pressed keys
    std::cout << "Keyboard State:" << std::endl;

    bool keysPressed = false;
    std::cout << "Pressed keys: ";
    for (int key = 0; key <= 255; key++) {
        if (g_Input.IsKeyDown(key)) {
            std::cout << key << " (" << g_Input.GetKeyName(key) << ") ";
            keysPressed = true;
        }
    }
    std::cout << std::endl;

    // Check function keys
    for (int key = VK_F1; key <= VK_F12; key++)
    {
        if (g_Input.IsKeyDown(key))
        {
            std::cout << g_Input.GetKeyName(key) << " ";
            keysPressed = true;
        }
    }

    // Check for special keys
    const int specialKeys[] = {
        VK_SHIFT, VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, VK_TAB, VK_ESCAPE,
        VK_SPACE, VK_RETURN, VK_BACK, VK_DELETE, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT,
        VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12
    };

    for (int key : specialKeys) {
        if (g_Input.IsKeyDown(key)) {
            std::cout << g_Input.GetKeyName(key) << " ";
            keysPressed = true;
        }
    }


    if (!keysPressed)
    {
        std::cout << "None";
    }

    std::cout << std::endl << std::endl;

    // Display gamepad state
    std::cout << "Gamepad State:" << std::endl;

    if (g_Input.IsGamepadAvailable())
    {
        const GamepadState& gamepad = g_Input.GetGamepadState();
        std::cout << "  Connected: " << g_Input.GetGamepadName() << std::endl;

        // Analog sticks
        std::cout << "  Left Stick: ("
            << std::fixed << std::setprecision(2) << gamepad.leftStickX << ", "
            << std::fixed << std::setprecision(2) << gamepad.leftStickY << ")" << std::endl;

        std::cout << "  Right Stick: ("
            << std::fixed << std::setprecision(2) << gamepad.rightStickX << ", "
            << std::fixed << std::setprecision(2) << gamepad.rightStickY << ")" << std::endl;

        // Triggers
        std::cout << "  Left Trigger: "
            << std::fixed << std::setprecision(2) << gamepad.leftTrigger << std::endl;

        std::cout << "  Right Trigger: "
            << std::fixed << std::setprecision(2) << gamepad.rightTrigger << std::endl;

        // Buttons
        std::cout << "  Buttons:" << std::endl;
        std::cout << "    A: " << (gamepad.buttons[GamepadButton::A] ? "Down" : "Up") << std::endl;
        std::cout << "    B: " << (gamepad.buttons[GamepadButton::B] ? "Down" : "Up") << std::endl;
        std::cout << "    X: " << (gamepad.buttons[GamepadButton::X] ? "Down" : "Up") << std::endl;
        std::cout << "    Y: " << (gamepad.buttons[GamepadButton::Y] ? "Down" : "Up") << std::endl;
        std::cout << "    Left Shoulder: " << (gamepad.buttons[GamepadButton::LEFT_SHOULDER] ? "Down" : "Up") << std::endl;
        std::cout << "    Right Shoulder: " << (gamepad.buttons[GamepadButton::RIGHT_SHOULDER] ? "Down" : "Up") << std::endl;
        std::cout << "    D-Pad Up: " << (gamepad.buttons[GamepadButton::DPAD_UP] ? "Down" : "Up") << std::endl;
        std::cout << "    D-Pad Right: " << (gamepad.buttons[GamepadButton::DPAD_RIGHT] ? "Down" : "Up") << std::endl;
        std::cout << "    D-Pad Down: " << (gamepad.buttons[GamepadButton::DPAD_DOWN] ? "Down" : "Up") << std::endl;
        std::cout << "    D-Pad Left: " << (gamepad.buttons[GamepadButton::DPAD_LEFT] ? "Down" : "Up") << std::endl;
    }
    else
    {
        std::cout << "  No gamepad detected" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
}

// Clean up DirectX objects
void CleanupDirectX()
{
    if (g_pDeviceContext) g_pDeviceContext->ClearState();

    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pDeviceContext) g_pDeviceContext->Release();
    if (g_pDevice) g_pDevice->Release();
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Pass input messages to the input handler
    g_Input.ProcessMessage(hwnd, message, wParam, lParam);

    switch (message)
    {
    case WM_SIZE:
        // Handle window resize
        if (g_pDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            if (g_pRenderTargetView) g_pRenderTargetView->Release();

            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);

            // Resize the swap chain
            g_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

            // Re-create the render target view
            ID3D11Texture2D* pBackBuffer = NULL;
            g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
            pBackBuffer->Release();

            // Reset render target
            g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

            // Set viewport
            D3D11_VIEWPORT vp;
            vp.Width = (FLOAT)width;
            vp.Height = (FLOAT)height;
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            vp.TopLeftX = 0;
            vp.TopLeftY = 0;
            g_pDeviceContext->RSSetViewports(1, &vp);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        g_Running = false;
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}