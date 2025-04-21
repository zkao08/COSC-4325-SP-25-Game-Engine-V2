#define STB_IMAGE_IMPLEMENTATION

#include "Renderer.h"
#include "Application.h"
#include "Window.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "stb_image.h"

#include <iostream>
#include <string>
#include <memory>
#include <map>

const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
static DirectX::XMVECTORF32 floatingVector = { 32.0f / 255.0f, 32.0f / 255.0f, 32.0f / 255.0f, 1.0f };

static std::map<char*, ID3D11ShaderResourceView*> cachedTextures;

Renderer::Renderer(Application* application) : m_Application(application) {}

Renderer::~Renderer() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Renderer::Create()
{
	int window_width, window_height;
	m_Application->GetWindow()->GetSize(&window_width, &window_height);

	// Setup Direct3D 11
	CreateDeviceAndContext();
	CreateSwapChain(window_width, window_height);
	CreateRenderTargetAndDepthStencilView(window_width, window_height);
	CreateBlendState();
	CreateSamplerState();
	SetViewport(window_width, window_height);

	// Setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureMouse = true;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	ImGui::StyleColorsDark(); // Dark theme
	ImGui_ImplDX11_Init(m_Device.Get(), m_DeviceContext.Get());
	ImGui_ImplWin32_Init(m_Application->GetWindow()->GetHwnd());
}

void Renderer::CreateDeviceAndContext()
{
	// Look for Direct3D 11 feature
	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	UINT feature_level_count = ARRAYSIZE(feature_levels);

	// Add debug flag if in debug mode
	D3D11_CREATE_DEVICE_FLAG deviceFlag = (D3D11_CREATE_DEVICE_FLAG)0;
#ifdef _DEBUG
	deviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create device and device context
	D3D_FEATURE_LEVEL feature_level;
	DX::Check(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlag, feature_levels, feature_level_count,
		D3D11_SDK_VERSION, m_Device.ReleaseAndGetAddressOf(), &feature_level, m_DeviceContext.ReleaseAndGetAddressOf()));

	// Check if Direct3D 11.1 is supported, if not fall back to Direct3D 11
	if (feature_level != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBox(NULL, "Unsupported", "D3D_FEATURE_LEVEL_11_1 is not supported! Falling back to D3D_FEATURE_LEVEL_11_0", MB_OK);
	}

	// Check if Direct3D 11 is supported
	if (feature_level != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBox(NULL, "Unsupported", "D3D_FEATURE_LEVEL_11_0 is not supported", MB_OK);
		throw std::exception();
	}
}

void Renderer::CreateSwapChain(int width, int height)
{
	// Query the device until we get the DXGIFactory
	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	DX::Check(m_Device.As(&dxgiDevice));

	ComPtr<IDXGIAdapter> adapter = nullptr;
	DX::Check(dxgiDevice->GetAdapter(adapter.GetAddressOf()));

	ComPtr<IDXGIFactory> dxgiFactory = nullptr;
	DX::Check(adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(dxgiFactory.GetAddressOf())));

	// Query IDXGIFactory to try to get IDXGIFactory2
	ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;
	DX::Check(dxgiFactory.As(&dxgiFactory2));

	// If we can support IDXGIFactory2 then use it to create the swap chain, otherwise fallback to IDXIFactory
	if (dxgiFactory2 != nullptr)
	{
		// DirectX 11.1
		DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
		swapchain_desc.Width = width;
		swapchain_desc.Height = height;
		swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchain_desc.SampleDesc.Count = 1;
		swapchain_desc.SampleDesc.Quality = 0;
		swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_desc.BufferCount = 2;
		swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		// CreateSwapChainForHwnd is the prefered way of creating the swap chain
		DX::Check(dxgiFactory2->CreateSwapChainForHwnd(m_Device.Get(), m_Application->GetWindow()->GetHwnd(), &swapchain_desc, nullptr, nullptr, &m_SwapChain1));
		DX::Check(m_SwapChain1.As(&m_SwapChain));
	}
	else
	{
		// Describe the swapchain
		DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
		swapchain_desc.BufferDesc.Width = width;
		swapchain_desc.BufferDesc.Height = height;
		swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchain_desc.SampleDesc.Count = 1;
		swapchain_desc.SampleDesc.Quality = 0;
		swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_desc.BufferCount = 2;
		swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		swapchain_desc.BufferDesc.RefreshRate.Numerator = 60;
		swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swapchain_desc.OutputWindow = m_Application->GetWindow()->GetHwnd();
		swapchain_desc.Windowed = TRUE;

		// Creates the swapchain
		DX::Check(dxgiFactory->CreateSwapChain(m_Device.Get(), &swapchain_desc, &m_SwapChain));
	}
}

void Renderer::CreateSamplerState() {
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	DX::Check(m_Device->CreateSamplerState(&samplerDesc, &m_SamplerState));
}

void Renderer::CreateRenderTargetAndDepthStencilView(int width, int height)
{
	// Create the render target view
	ComPtr<ID3D11Texture2D> back_buffer = nullptr;
	DX::Check(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(back_buffer.GetAddressOf())));
	DX::Check(m_Device->CreateRenderTargetView(back_buffer.Get(), nullptr, m_RenderTargetView.GetAddressOf()));

	// Describe the depth stencil view
	D3D11_TEXTURE2D_DESC depth_desc = {};
	depth_desc.Width = width;
	depth_desc.Height = height;
	depth_desc.MipLevels = 1;
	depth_desc.ArraySize = 1;
	depth_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_desc.SampleDesc.Count = 1;
	depth_desc.SampleDesc.Quality = 0;
	depth_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// Create the depth stencil view
	ComPtr<ID3D11Texture2D> depth_stencil = nullptr;
	DX::Check(m_Device->CreateTexture2D(&depth_desc, nullptr, &depth_stencil));
	DX::Check(m_Device->CreateDepthStencilView(depth_stencil.Get(), nullptr, m_DepthStencilView.GetAddressOf()));

	// Binds both the render target and depth stencil to the pipeline's output merger stage
	m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
}

void Renderer::CreateBlendState() {
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = TRUE; // Fun fact: Setting this to FALSE punches a hole through the viewport.
	blendDesc.IndependentBlendEnable = FALSE;

	D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[0];
	rtBlendDesc.BlendEnable = TRUE;
	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DX::Check(m_Device->CreateBlendState(&blendDesc, m_BlendState.ReleaseAndGetAddressOf()));
}

void Renderer::SetViewport(int width, int height)
{
	// Describe the viewport
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	// Bind viewport to the pipline's rasterization stage
	m_DeviceContext->RSSetViewports(1, &viewport);
}

void Renderer::Clear()
{
	// Clear the render target view to the chosen colour
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), floatingVector);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Bind the render target view to the pipeline's output merger stage
	m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Renderer::Present()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Use IDXGISwapChain1::Present1 for presenting instead
	// This is a requirement for using variable refresh rate displays
	if (m_SwapChain1 != nullptr)
	{
		BOOL fullscreen;
		DX::Check(m_SwapChain1->GetFullscreenState(&fullscreen, nullptr));

		DXGI_PRESENT_PARAMETERS presentParameters = {};
		UINT present_flags = (fullscreen ? 0 : DXGI_PRESENT_ALLOW_TEARING);
		DX::Check(m_SwapChain1->Present1(0, present_flags, &presentParameters));
	}
	else
	{
		DX::Check(m_SwapChain->Present(0, 0));
	}
}

void Renderer::Resize(int width, int height)
{
	// Can only resize if width or height has a positive value to avoid crashing
	if (width <= 0 || height <= 0)
		return;

	// Releases the current render target and depth stencil view
	m_DepthStencilView.ReleaseAndGetAddressOf();
	m_RenderTargetView.ReleaseAndGetAddressOf();

	// Resize the swapchain
	DX::Check(m_SwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING));

	// Creates a new render target and depth stencil view with the new window size
	CreateRenderTargetAndDepthStencilView(width, height);

	// Sets a new viewport with the new window size
	SetViewport(width, height);
}

// Loads an image into a DX11 texture with common settings
bool Renderer::LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	m_Device->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	m_Device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

// Opens and reads a file, then forwards to LoadTextureFromMemory()
bool Renderer::LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	FILE* f = fopen(file_name, "rb");
	if (f == NULL)
		return false;
	fseek(f, 0, SEEK_END);
	size_t file_size = (size_t)ftell(f);
	if (file_size == -1)
		return false;
	fseek(f, 0, SEEK_SET);
	void* file_data = IM_ALLOC(file_size);
	fread(file_data, 1, file_size, f);
	fclose(f);
	bool ret = LoadTextureFromMemory(file_data, file_size, out_srv, out_width, out_height);
	IM_FREE(file_data);
	return ret;
}

// Creates an image button with a provided ID name, file path, and button size.
bool Renderer::CreateImageButton(char* id, char* path, ImVec2 size) {
	ID3D11ShaderResourceView* image = NULL;
	int size_x = size.x;
	int size_y = size.y;
	bool imageResult = true;
	bool pressed = false;

	// Fun fact: Not caching textures here results in a HUGE memory leak, meaning the computer can literally crash in minutes from running out of memory.
	if (cachedTextures[path] != NULL)
		image = cachedTextures[path];
	else {
		imageResult = LoadTextureFromFile(path, &image, &size_x, &size_y);
		cachedTextures[path] = image;
	}

	if (imageResult) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));

		pressed = ImGui::ImageButton(id, (ImTextureID)(intptr_t)image, ImVec2(size.x, size.y));

		ImGui::PopStyleColor(3);
	}
	else
		pressed = ImGui::Button(id);

	return pressed;
}

float Renderer::GetScaleFactor(float width, float height) {
	return min(((float)(GetSystemMetrics(SM_CXSCREEN) * 2) / width), ((float)(GetSystemMetrics(SM_CYSCREEN) * 2) / height));
}

ComPtr<ID3D11Device> Renderer::GetDevice() {
	return m_Device;
}
ComPtr<ID3D11DeviceContext> Renderer::GetContext() {
	return m_DeviceContext;
}

ComPtr<ID3D11BlendState> Renderer::GetBlendState() {
	return m_BlendState;
}

ComPtr<ID3D11SamplerState> Renderer::GetSamplerState() {
	return m_SamplerState;
}