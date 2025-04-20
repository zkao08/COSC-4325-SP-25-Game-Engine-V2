#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Window.h"
#include "Timer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Rect.h"
#include "Camera.h"
#include "RasterState.h"
#include "RenderTarget.h"
#include "Game.h"

#include <memory>
#include <string>

class Window;
class Renderer;

class Application
{
	private:
		std::unique_ptr<Window> m_Window = nullptr;
		std::unique_ptr<Renderer> m_Renderer = nullptr;
		std::unique_ptr<Shader> m_Shader = nullptr;
		std::unique_ptr<Camera> m_Camera = nullptr;
		std::unique_ptr<Camera> m_CameraPlane = nullptr;
		std::unique_ptr<RasterState> m_RasterState = nullptr;
		std::unique_ptr<RenderTarget> m_RenderTarget = nullptr;
		std::unique_ptr<Game> m_Game = nullptr;

		bool m_Running = true;
		bool m_WindowCreated = false;
		std::string m_ApplicationTitle = "App";

		// On resized event
		void OnResized(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void OnMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, float delta_z = 0.0f);

		void OnMouseScroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void OnKeyDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void ComputePlaneViewProjectionMatrix();

		void RenderToTexture();

		void RenderToBackBuffer();

		// Calculate frame stats
		void CalculateFrameStats(float delta_time);
		int m_FrameCount = 0;

		void ComputeModelViewProjectionMatrix();

		void GetResolution(int& x, int& y);

	public:
		Application();
		virtual ~Application() = default;

		int Execute();

		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// Get window
		inline Window* GetWindow() const { return m_Window.get(); }
};