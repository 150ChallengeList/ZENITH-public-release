#ifndef WINDOWMANAGER_IMGUI_HPP
#define WINDOWMANAGER_IMGUI_HPP

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui.h"
#include "BaseLib.hpp"
#include <Windows.h>
#include <d3d11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
class WindowManager_ImGui : public BaseLib_t {
public:

	void onInit(ID3D11Device* D3D11Device, ID3D11DeviceContext* D3D11DeviceContext, IDXGISwapChain* DXGISwapChain, ID3D11RenderTargetView* D3D11RenderTargetView, HWND hWnd) override {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO* io = &ImGui::GetIO();
		io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(D3D11Device, D3D11DeviceContext);
	}

	void onBegin() override {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void onEnd() override {
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	int onWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return 1;
		return 0;
	}

};

#endif WINDOWMANAGER_IMGUI_HPP