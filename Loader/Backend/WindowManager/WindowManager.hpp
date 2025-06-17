#ifndef WINDOWMANAGER_HPP
#define WINDOWMANAGER_HPP

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <unordered_map>
#include <functional>
#include <Windows.h>
#include <d3d11.h>
#include <string>
#include <atomic>

class Window {
private:

    bool createDeviceAndSwapChain();
    void destroyDeviceAndSwapChain();

    bool createImGui();
    bool createRenderTarget();

    static LRESULT WINAPI wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    std::function<void()> renderFunction;

    CRITICAL_SECTION presentSection;
    std::atomic<bool> shouldRender = false;

public:
    bool shouldClose = false;

    ID3D11Device* d3d11Device;
    IDXGISwapChain* dxgiSwapChain;
    ID3D11DeviceContext* d3d11DeviceContext;
    ID3D11RenderTargetView* d3d11RenderTargetView;

    WNDCLASSEXW windowClass;
    HWND windowHandle;

    bool initWindow(HINSTANCE instance, const std::wstring& windowClassName, int x, int y, int cx, int cy, std::function<void()> renderFn);
    bool deinitWindow();

    void render();
    void present();

    void update();
};

#endif WINDOWMANAGER_HPP