#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "Application.h"
#include "Utility.h"

#include <Windows.h>
#include <string>

class Application;

class Window {
	private:
		Application* m_Application = nullptr;
		HWND m_Hwnd = NULL;
		HINSTANCE m_Hinstance = nullptr;
		std::string m_Title;
	public:
		Window(Application* application);
		virtual ~Window();

		// Creates the window
		bool Create(const std::string title, int width, int height, bool fullscreen);

		// Destroys the window
		void Destroy();

		// Gets window size
		void GetSize(int* width, int* height);
		void GetSize(int& width, int& height);

		// Gets native window handle
		inline HWND GetHwnd() const { return m_Hwnd; }

		// Gets application
		inline Application* GetApplication() const { return m_Application; }

		// Sets title
		void SetTitle(const std::string& title);
};