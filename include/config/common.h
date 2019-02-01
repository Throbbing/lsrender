#pragma once
#include<config/config.h>
#include<string>

namespace ls
{
	class lsEmbree
	{
	public:
		
		
		static struct _hw{
			RTCDevice			rtcDevice = nullptr;
			RTCScene			rtcScene = nullptr;
		}hw;
		
		static void initEmbree();
		static void releaseEmbree();

	private:
		static bool				isInit;
	};

#ifndef HR

#define  HR(x)																			\
{																						\
	HRESULT hr = (x);																	\
if (FAILED(hr))																		\
	{																					\
	DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);								\
	}																					\
}
#endif
	class lsWnd
	{
	public:

		static
			struct _wnd
		{
			HWND									wndHwnd = nullptr;
			HINSTANCE								wndHinstance = nullptr;
			u32										screenWidth;
			u32										screenHeight;
			f32										fps = 0.f;
		}wnd;

		static
			struct _hw
		{
			ID3D11Device*							d3dDevice = nullptr;
			ID3D11DeviceContext*					d3dImmediateContext = nullptr;
			IDXGISwapChain*							swapChain = nullptr;
			ID3D11Texture2D*						depthStencilBuffer = nullptr;

			ID3D11Texture2D*						backBuffer = nullptr;
			ID3D11RenderTargetView*					renderTargetView = nullptr;
			ID3D11DepthStencilView*					depthStencilView = nullptr;
			D3D11_VIEWPORT							viewPort;
		}hw;



		static
			bool										lsWndInit(u32 w, u32 h, WNDPROC wndProc);

		static
			void										lsWndShutDown();


	};

	
	



}


