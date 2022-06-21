#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <string>
#include "Timer.h"
#include "Input.h"
#pragma comment (lib, "d3d11.lib")

class D3DFramwork
{
	const std::wstring CLASSNAME{ L"MyWindowClass" };
	const std::wstring TITLENAME{ L"Direct3D" };

protected :
	int mScreenWidth{ 800 };
	int mScreenHeight{ 600 };

	HWND mHwnd{};
	HINSTANCE mInstance{};

	bool mMinimized{ false };
	bool mMaximized{ false };
	bool mResizing{ false };
	bool mPaused{ false };

	MyUtil::Timer mTimer;
	MyUtil::Input mInput;
	std::wstring mTitleText{};

protected :
	Microsoft::WRL::ComPtr<ID3D11Device>             mspDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>      mspDeviceContext{};
	Microsoft::WRL::ComPtr<IDXGISwapChain>           mspSwapChain{};

	Microsoft::WRL::ComPtr<ID3D11Texture2D>          mspRenderTarget{};
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   mspRenderTargetView{};
	Microsoft::WRL::ComPtr<ID3D11Texture2D>          mspDepthStencil{};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   mspDepthStencilView{};

private :
	void CalculateFPS();

protected :
	void InitWindow(HINSTANCE hInstance);
	void InitD3D();
	void OnResize();
	void RenderFrame();

	virtual void Render() {};
	virtual void Update(float delta) {};

public :
	virtual void Initialize(HINSTANCE hInstance, int width = 800, int height = 600);
	virtual void Destroy();
	void GameLoop();

public :
	LRESULT CALLBACK MessageHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);