#include "Application.h"

const int TARGET_RESOLUTION_X = 1920;
const int TARGET_RESOLUTION_Y = 1080;

static int scaledResolutionX;
static int scaledResolutionY;

Application::Application(std::string title, Object* game_object, bool dev_mode) {
	GetResolution(scaledResolutionX, scaledResolutionY);

	m_ApplicationTitle = title;
	m_DevMode = dev_mode;

	scaledResolutionX *= 2;
	scaledResolutionY *= 2;

	scaledResolutionX = abs(TARGET_RESOLUTION_X - scaledResolutionX);
	scaledResolutionY = abs(TARGET_RESOLUTION_Y - scaledResolutionY);

	// Create window
	m_Window = std::make_unique<Window>(this);
	m_WindowCreated = m_Window->Create(title.c_str(), TARGET_RESOLUTION_X, TARGET_RESOLUTION_Y, false);

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
	m_RenderTarget->Create(TARGET_RESOLUTION_X, TARGET_RESOLUTION_Y);

	// Create raster state
	m_RasterState = std::make_unique<RasterState>(m_Renderer.get());

	// Create game state
	m_Game = std::make_unique<Game>(game_object, m_Renderer.get());
}

int Application::Initialize() {
	scaleFactor = m_Renderer->GetScaleFactor((float)scaledResolutionX, (float)scaledResolutionY);

	return 1;
}

int Application::Render(float deltaTime) {
	int status = NONE;

	this->CalculateFrameStats(deltaTime);

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

		// Binds the render-to-texture render target to the pipeline
		m_RenderTarget->Use();
		// Update the model view projection constant buffer
		this->ComputeModelViewProjectionMatrix();

		if (m_DevMode) {
			m_Game->GetGameObject()->Update(true);
			m_Renderer->Clear();

			Dock::SetDockingBehavior();

			int result = MainMenuBar::Render();
			if (result == CLOSE_APP)
				status = CLOSE_APP;
			else if (result == RUN_GAME) {
				GameEngine::CreateRuntime("Runtime", m_Game->GetGameObject());
			}

			NavigatorWindow::Render(m_Renderer.get(), m_Game.get(), scaleFactor);
			PropertiesWindow::Render(m_Game.get(), scaleFactor);
			ViewportWindow::Render(m_Renderer.get(), scaleFactor, (ImTextureID)(intptr_t)m_RenderTarget->GetTexture(), m_Camera.get());
			ObjectWindow::Render(m_Renderer.get(), m_Game.get(), scaleFactor);

			ImVec2 viewportWindowSize = ViewportWindow::GetSize();
			m_Camera->UpdateAspectRatio((int)viewportWindowSize.x, (int)viewportWindowSize.x);
			m_CameraPlane->UpdateAspectRatio((int)viewportWindowSize.x, (int)viewportWindowSize.x);
		}
		else {
			m_Renderer->Clear();
			m_Game->GetGameObject()->Update(false);
			m_Game->GetPhysicsWorld()->Step(deltaTime);
		}

		// Display the rendered scene
		m_Renderer->Present();
	}

	return status;
}

LRESULT Application::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_DESTROY:
		if (m_DevMode)
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

	case WM_LBUTTONDOWN:
		this->OnMouseDown(hwnd, msg, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
		this->OnKeyDown(hwnd, msg, wParam, lParam);
		return 0;
	}

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

	// Resize render target
	m_RenderTarget->Create(window_width, window_height);

	m_Camera->UpdateAspectRatio(window_width, window_height);
	m_CameraPlane->UpdateAspectRatio(window_width, window_height);
}

void Application::OnMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, float delta_z) {
	if (m_DevMode && !ViewportWindow::IsHovered())
		return;

	static int previous_mouse_x = 0;
	static int previous_mouse_y = 0;

	int mouse_x = static_cast<int>(GET_X_LPARAM(lParam));
	int mouse_y = static_cast<int>(GET_Y_LPARAM(lParam));

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

	m_MouseX = mouse_x;
	m_MouseY = mouse_y;
}

void Application::OnMouseScroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (m_DevMode && !ViewportWindow::IsHovered())
		return;

	static int previous_mouse_z = 0;

	int mouse_z = static_cast<int>(GET_WHEEL_DELTA_WPARAM(wParam));
	float relative_mouse_z = static_cast<float>(mouse_z - previous_mouse_z);
	float delta_z = relative_mouse_z * 0.01f;

	this->OnMouseMove(hwnd, msg, wParam, lParam, delta_z);
}

void Application::OnMouseDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (m_DevMode && !ViewportWindow::IsHovered())
		return;

	int mouse_x = static_cast<int>(GET_X_LPARAM(lParam));
	int mouse_y = static_cast<int>(GET_Y_LPARAM(lParam));

	float x;
	float y;

	MouseToWorldCoordinates(x, y);

	std::cout << x << " " << y << std::endl;
}

void Application::OnKeyDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (m_DevMode && !ViewportWindow::IsFocused())
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

	m_ProjectionMatrix = matrix;
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
	//this->ComputePlaneViewProjectionMatrix();
}

void Application::GetResolution(int& x, int& y)
{
	RECT desktop;

	const HWND hDesktop = GetDesktopWindow();

	GetWindowRect(hDesktop, &desktop);

	x = desktop.right;
	y = desktop.bottom;
}

void Application::MouseToWorldCoordinates(float& world_x, float& world_y) {
	int screenWidth;
	int screenHeight;

	//m_Window->GetSize(screenWidth, screenHeight);

	ImVec2 size = ViewportWindow::GetSize();
	screenWidth = size.x;
	screenHeight = size.y;

	float mousex = (m_MouseX - size.x) - (m_Camera->GetPosition().x - size.x / 2);
	float mousey = (m_MouseY - size.y) - (m_Camera->GetPosition().y - size.y / 2);

	MouseToWorldCoordinates(mousex, mousey, m_Window->GetHwnd(), screenWidth, screenHeight, m_ProjectionMatrix, m_Camera->GetProjection(), world_x, world_y);
}

void Application::MouseToWorldCoordinates(int mouse_x, int mouse_y, HWND window, int screen_width, int screen_height, const DirectX::XMMATRIX& projection_matrix, const DirectX::XMMATRIX& view_matrix, float& world_x, float& world_y) {
	RECT mainRect;
	GetClientRect(window, &mainRect);
	int mainWindowWidth = mainRect.right - mainRect.left;
	int mainWindowHeight = mainRect.bottom - mainRect.top;

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(window, &mousePos);
	mouse_x = mousePos.x;
	mouse_y = mousePos.y;

	mouse_x -= ViewportWindow::GetSize().x;
	mouse_y -= ViewportWindow::GetSize().y;

	// Normalize mouse coordinates
	float normalizedX = (2.0f * mouse_x) / screen_width - 1.0f;
	float normalizedY = 1.0f - (2.0f * mouse_y) / screen_height; // Invert Y

	// Create NDC vector
	DirectX::XMFLOAT4 mouseNDC(normalizedX, normalizedY, 0.0f, 1.0f);

	// Inverse projection matrix
	DirectX::XMMATRIX inverseProjection = DirectX::XMMatrixInverse(nullptr, projection_matrix);

	// Inverse view matrix
	DirectX::XMMATRIX inverseView = DirectX::XMMatrixInverse(nullptr, view_matrix);

	// Transform to clip space
	DirectX::XMVECTOR clipSpace = DirectX::XMLoadFloat4(&mouseNDC);

	// Transform to world coordinates
	DirectX::XMVECTOR worldSpace = DirectX::XMVector3Transform(clipSpace, inverseProjection);
	worldSpace = DirectX::XMVector3Transform(worldSpace, inverseView);

	// Extract world coordinates
	world_x = DirectX::XMVectorGetX(worldSpace);
	world_y = DirectX::XMVectorGetY(worldSpace);
}