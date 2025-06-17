#ifndef BASELIB_HPP
#define BASELIB_HPP

#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <string>
#include <vector>

class BaseLib_t {
private:

public:

	virtual void onFileDrop(const std::vector<std::string>& filePaths) {};

	virtual int onWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return 0; };

	virtual void onInit(ID3D11Device* D3D11Device, ID3D11DeviceContext* D3D11DeviceContext, IDXGISwapChain* DXGISwapChain, ID3D11RenderTargetView* D3D11RenderTargetView, HWND hWnd) {};

	virtual void onBegin() {};
	virtual void onEnd() {};

	virtual void onDeinit() {};

};

#endif BASELIB_HPP