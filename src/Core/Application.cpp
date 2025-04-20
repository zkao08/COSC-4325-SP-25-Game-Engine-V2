#include "Application.h"
#include "Window.h"
#include "Timer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Rect.h"
#include "Camera.h"
#include "RasterState.h"
#include "RenderTarget.h"
#include "Grid.h"
#include "Game.h"

#include "Dock.h"
#include "EntityWindow.h"
#include "MainMenuBar.h"
#include "NavigatorWindow.h"
#include "PropertiesWindow.h"
#include "ViewportWindow.h"

#include <DirectXMath.h>
#include <windowsx.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <math.h>

const int TARGET_RESOLUTION_X = 1920;
const int TARGET_RESOLUTION_Y = 1080;

static int scaledResolutionX;
static int scaledResolutionY;

static float scaleFactor;

Application::Application() {
	GetResolution(scaledResolutionX, scaledResolutionY);

	scaledResolutionX *= 2;
	scaledResolutionY *= 2;

	scaledResolutionX = abs(TARGET_RESOLUTION_X - scaledResolutionX);
	scaledResolutionY = abs(TARGET_RESOLUTION_Y - scaledResolutionY);

	// Create window
	m_Window = std::make_unique<Window>(this);
	m_WindowCreated = m_Window->Create(m_ApplicationTitle.c_str(), TARGET_RESOLUTION_X, TARGET_RESOLUTION_Y, false);

	// Create renderer
	m_Renderer = std::make_unique<Renderer>(this);
	m_Renderer->Create();

	// Create shader
	m_Shader = std::make_unique<Shader>(m_Renderer.get());
	m_Shader->Load();

	// Create camera
	m_Camera = std::make_unique<Camera>(scaledResolutionX, scaledResolutionY);
	m_CameraPlane = std::make_unique<Camera>(scaledResolutionX, scaledResolutionY);

	// Create render target
	m_RenderTarget = std::make_unique<RenderTarget>(m_Renderer.get());
	m_RenderTarget->Create(scaledResolutionX, scaledResolutionY);

	// Create game state
	m_Game = std::make_unique<Game>();
}

int Application::Execute() {
	int result = 0;

	Timer timer;
	timer.Start();

	scaleFactor = m_Renderer->GetScaleFactor((float)scaledResolutionX, (float)scaledResolutionY);

	// Rect
	std::unique_ptr<Rect> newRect = std::make_unique<Rect>(m_Renderer.get());
	newRect->Create(L"../../../assets/WoodTexture.jpg");

	// Grid
	//std::unique_ptr<Grid> newGrid = std::make_unique<Grid>(m_Renderer.get());
	//newGrid->Create(1000, 2.0f);

	// Raster state
	m_RasterState = std::make_unique<RasterState>(m_Renderer.get());

	m_Game.get()->CreateTestItems();

	// Main application loop
	while (m_Running)
	{
		timer.Tick();
		this->CalculateFrameStats(timer.DeltaTime());

		MSG msg = {};
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				m_Running = false;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else {
			m_Shader->Use();
			m_RasterState->Use();

			//RenderToTexture();
			// Binds the render-to-texture render target to the pipeline
			m_RenderTarget->Use();
			// Update the model view projection constant buffer
			this->ComputeModelViewProjectionMatrix();
			// Render the model
			newRect->Render();
			//newGrid->Render();

			//RenderToBackBuffer();
			m_Renderer->Clear();

			SetDockingBehavior();

			result = MainMenuBar::Render();
			if (result != 1)
				break;

			NavigatorWindow::Render(m_Renderer.get(), m_Game.get(), scaleFactor);
			PropertiesWindow::Render(m_Game.get(), scaleFactor);
			ViewportWindow::Render(m_Renderer.get(), scaleFactor, (ImTextureID)(intptr_t)m_RenderTarget->GetTexture());
			EntityWindow::RenderEntityWindow(scaleFactor);

			ImVec2 viewportWindowSize = ViewportWindow::GetSize();
			m_Camera->UpdateAspectRatio(viewportWindowSize.x, viewportWindowSize.y);

			// Display the rendered scene
			m_Renderer->Present();
		}
	}

	return 0;
}

LRESULT Application::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		this->OnResized(hwnd, msg, wParam, lParam);
		return 0;

	case WM_MOUSEMOVE:
		this->OnMouseMove(hwnd, msg, wParam, lParam);
		return 0;

	case WM_MOUSEWHEEL:
		this->OnMouseScroll(hwnd, msg, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
		this->OnKeyDown(hwnd, msg, wParam, lParam);
		return 0;
	}

	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Application::OnResized(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// Window resized is called upon window creation, so ignore if the window has not finished being created
	if (!m_WindowCreated)
		return;

	// Get window size
	int window_width = LOWORD(lParam);
	int window_height = HIWORD(lParam);

	// Resize renderer
	m_Renderer->Resize(window_width, window_height);
}

void Application::OnMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, float delta_z) {
	static int previous_mouse_x = 0;
	static int previous_mouse_y = 0;

	int mouse_x = static_cast<int>(GET_X_LPARAM(lParam));
	int mouse_y = static_cast<int>(GET_Y_LPARAM(lParam));

	/*if (wParam & MK_LBUTTON) {
		float relative_mouse_x = static_cast<float>(mouse_x - previous_mouse_x);
		float relative_mouse_y = static_cast<float>(mouse_y - previous_mouse_y);

		// Rotate camera
		float yaw = relative_mouse_x * 0.01f;
		float pitch = relative_mouse_y * 0.01f;

		m_Camera->Rotate(pitch, yaw);
	}*/
	if ((wParam & MK_MBUTTON)) {
		float relative_mouse_x = static_cast<float>(mouse_x - previous_mouse_x);
		float relative_mouse_y = static_cast<float>(mouse_y - previous_mouse_y);

		float delta_x = relative_mouse_x * 0.01f;
		float delta_y = relative_mouse_y * 0.01f;

		m_Camera->Move(delta_x, -delta_y, 0.0f);
	}
	else if (delta_z != 0.0f) {
		m_Camera->Move(0.0f, 0.0f, -delta_z);
	}

	previous_mouse_x = mouse_x;
	previous_mouse_y = mouse_y;
}

void Application::OnMouseScroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static int previous_mouse_z = 0;

	int mouse_z = static_cast<int>(GET_WHEEL_DELTA_WPARAM(wParam));
	float relative_mouse_z = static_cast<float>(mouse_z - previous_mouse_z);
	float delta_z = relative_mouse_z * 0.01f;

	this->OnMouseMove(hwnd, msg, wParam, lParam, delta_z);
}

void Application::OnKeyDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//WORD flags = HIWORD(lParam);
	//BOOL key_repeat = (flags & KF_REPEAT) == KF_REPEAT;

	if (!ViewportWindow::IsFocused())
		return;

	if (GetKeyState('R') & 0x8000) {
		m_Camera->Reset();
		m_Camera->Move(0.0f, 0.0f, 0.0f);
	}
	else if (GetKeyState('W') & 0x8000)
		m_RasterState->ToggleWireframe();
}

void Application::CalculateFrameStats(float delta_time) {
	static float time = 0.0f;

	m_FrameCount++;
	time += delta_time;

	// Update window title every second with FPS
	if (time > 1.0f)
	{
		std::string frame_title = "(FPS: " + std::to_string(m_FrameCount) + ")";
		m_Window->SetTitle(m_ApplicationTitle + " " + frame_title);

		time = 0.0f;
		m_FrameCount = 0;
	}
}

void Application::ComputeModelViewProjectionMatrix()
{
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	matrix *= m_Camera->GetView();
	matrix *= m_Camera->GetProjection();

	m_Shader->UpdateModelViewProjectionBuffer(matrix);
}

void Application::ComputePlaneViewProjectionMatrix()
{
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	matrix *= DirectX::XMMatrixTranslation(-2.0f, 2.0f, 0.0f);

	matrix *= m_CameraPlane->GetView();
	matrix *= m_CameraPlane->GetProjection();

	m_Shader->UpdateModelViewProjectionBuffer(matrix);
}

void Application::RenderToTexture()
{
	// Binds the render-to-texture render target to the pipeline
	m_RenderTarget->Use();

	// Update the model view projection constant buffer
	this->ComputeModelViewProjectionMatrix();

	// Render the model
	//m_Model->Render();
}

void Application::RenderToBackBuffer()
{
	// Clear the buffers and bind's the backbuffer as the render target
	m_Renderer->Clear();

	// Update the model view projection constant buffer and renders the model
	//this->ComputeModelViewProjectionMatrix();
	//m_Model->Render();

	// Sets the plane's texture to be the render-to-texture texture
	//m_Plane->SetTexture(m_RenderTarget->GetTexture());

	// Update the plane view projection constant buffer and renders the plane
	//this->ComputePlaneViewProjectionMatrix();
	//m_Plane->Render();
}

void Application::GetResolution(int& x, int& y)
{
	RECT desktop;

	const HWND hDesktop = GetDesktopWindow();

	GetWindowRect(hDesktop, &desktop);

	x = desktop.right;
	y = desktop.bottom;
}