#include "WindowManager.hpp"
#include <iostream>
#pragma comment(lib, "d3d11")

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <dwmapi.h>
#include <windows.h>
#include <chrono>

bool Window::createDeviceAndSwapChain() {

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 100;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; 
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;

    swapChainDesc.OutputWindow = this->windowHandle;
    swapChainDesc.Windowed = TRUE;

    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; 
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &swapChainDesc, &this->dxgiSwapChain, &this->d3d11Device, &featureLevel, &this->d3d11DeviceContext);

    if (result == DXGI_ERROR_UNSUPPORTED)
        result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &swapChainDesc, &this->dxgiSwapChain, &this->d3d11Device, &featureLevel, &this->d3d11DeviceContext);

    if (result != S_OK)
        return false;

    return true;
}
void Window::destroyDeviceAndSwapChain() {
    if (this->d3d11Device)
        this->d3d11Device->Release();
    if (this->d3d11DeviceContext)
        this->d3d11DeviceContext->Release();
    if (this->dxgiSwapChain)
        this->dxgiSwapChain->Release();
}

bool Window::createImGui() {

    IMGUI_CHECKVERSION();
    if (ImGui::CreateContext() == nullptr)
        return false;
    
    ImGuiIO* io = &ImGui::GetIO();
    
    if (!ImGui_ImplWin32_Init(this->windowHandle))
        return false;
    
    if (!ImGui_ImplDX11_Init(this->d3d11Device, this->d3d11DeviceContext))
        return false;
    
    return true;
}
bool Window::createRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    this->dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

    if (!pBackBuffer)
        return false;

    HRESULT result = this->d3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &this->d3d11RenderTargetView);
    pBackBuffer->Release();

    return result == S_OK;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI Window::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
   
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return 0;
    
    switch (msg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool Window::initWindow(HINSTANCE instance, const std::wstring& windowClassName, int x, int y, int cx, int cy, std::function<void()> renderFn) {
    this->windowClass.cbSize = sizeof(WNDCLASSEXW);
    this->windowClass.hInstance = instance;
    this->windowClass.lpfnWndProc = wndProc;
    this->windowClass.style = CS_HREDRAW | CS_VREDRAW;
    this->windowClass.lpszClassName = L"ZENITH";

    RegisterClassExW(&this->windowClass);

    this->windowHandle = CreateWindowExW(
        NULL, //WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
        L"ZENITH",
        nullptr,
        WS_POPUP,
        x, y, cx, cy,
        nullptr, nullptr, instance, nullptr);

    if (!this->windowHandle)
        return false;

    // Set the window to be transparent
    //SetLayeredWindowAttributes(this->windowHandle, 0, 0, LWA_COLORKEY);

    this->renderFunction = renderFn;

    UpdateWindow(this->windowHandle);
    ShowWindow(this->windowHandle, SW_SHOW);

    if (!this->createDeviceAndSwapChain()) {
        UnregisterClassW(this->windowClass.lpszClassName, this->windowClass.hInstance);
        return false;
    }

    if (!this->createRenderTarget()) {
        this->destroyDeviceAndSwapChain();
        UnregisterClassW(this->windowClass.lpszClassName, this->windowClass.hInstance);
        return false;
    }

    if (!this->createImGui()) {
        this->destroyDeviceAndSwapChain();
        UnregisterClassW(this->windowClass.lpszClassName, this->windowClass.hInstance);
        return false;
    }

    InitializeCriticalSection(&this->presentSection);

    return true;
}

bool Window::deinitWindow() {

    if (this->d3d11Device)
        this->d3d11Device->Release();

    if (this->d3d11DeviceContext)
        this->d3d11DeviceContext->Release();

    if (this->dxgiSwapChain)
        this->d3d11Device->Release();


    return true;
}

void Window::render() {
    if (!TryEnterCriticalSection(&this->presentSection))
        return;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    this->renderFunction();

    ImGui::Render();

    constexpr float clearColor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
    this->d3d11DeviceContext->OMSetRenderTargets(1, &this->d3d11RenderTargetView, nullptr);
    this->d3d11DeviceContext->ClearRenderTargetView(this->d3d11RenderTargetView, clearColor);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    LeaveCriticalSection(&this->presentSection);
    this->shouldRender.store(true);
}
void Window::present() {
    if (!this->shouldRender.load())
        return;

    EnterCriticalSection(&this->presentSection);
    this->dxgiSwapChain->Present(0, 0);
    LeaveCriticalSection(&this->presentSection);
}

void Window::update() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}