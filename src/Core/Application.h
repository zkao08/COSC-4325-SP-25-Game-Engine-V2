// Application Class
// Handles creating and rendering the game engine level editor and runtime.

#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "GameEngine.h"

#include "Window.h"
#include "Timer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Rect.h"
#include "Camera.h"
#include "RasterState.h"
#include "RenderTarget.h"
#include "Game.h"

#include "Object.h"

#include "Dock.h"
#include "ObjectWindow.h"
#include "MainMenuBar.h"
#include "NavigatorWindow.h"
#include "PropertiesWindow.h"
#include "ViewportWindow.h"

#include <DirectXMath.h>
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <vector>
#include <memory>
#include <math.h>

class Window;
class Renderer;
class Game;
class Camera;

class Application {
	private:
		// Components
		std::unique_ptr<Window> m_Window = nullptr;
		std::unique_ptr<Renderer> m_Renderer = nullptr;
		std::unique_ptr<Shader> m_Shader = nullptr;
		std::unique_ptr<Camera> m_Camera = nullptr;
		std::unique_ptr<Camera> m_CameraPlane = nullptr;
		std::unique_ptr<RasterState> m_RasterState = nullptr;
		std::unique_ptr<RenderTarget> m_RenderTarget = nullptr;
		std::unique_ptr<Game> m_Game = nullptr;

		// States
		bool m_Running = true;
		bool m_WindowCreated = false;
		std::string m_ApplicationTitle = "App";

		// System events
		void OnResized(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void OnMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, float delta_z = 0.0f);
		void OnMouseScroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void OnKeyDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// Projection matrix calculations
		void ComputePlaneViewProjectionMatrix();
		void ComputeModelViewProjectionMatrix();

		// Renders world into texture
		void RenderToTexture();

		// Frame stats
		void CalculateFrameStats(float delta_time);
		int m_FrameCount = 0;

	public:
		// Variables
		bool m_DevMode;
		float scaleFactor = 0.0f;

		int m_MouseX;
		int m_MouseY;

		DirectX::XMMATRIX m_ProjectionMatrix;

		// Constructor and Destructor
		Application(std::string title = "App", Object* game_object = nullptr, bool dev_mode = false);
		virtual ~Application() = default;

		int Initialize();
		int Render(float deltaTime = 0.0f);

		// System events
		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// Get window properties
		inline Window* GetWindow() const { return m_Window.get(); }
		void GetResolution(int& x, int& y);
};