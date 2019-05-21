 #include<config\common.h>
#include<d3d11.h>
#include<DirectXMath.h>
#include<D3dcsx.h>
#include<d3dCompiler.h>
#include<3rd/DirectX/WICTextureLoader.h>
#include<3rd/DirectX/dxerr.h>
#include<3rd/DirectX/DirectXTex.h>

#include<windows.h>
#include<tchar.h>
bool ls::lsEmbree::isInit = false;
ls::lsEmbree::_hw ls::lsEmbree::hw;
void ls::lsEmbree::initEmbree()
{
	if (isInit)
		return;

	hw.rtcDevice = rtcNewDevice(nullptr);
	hw.rtcScene = rtcNewScene(hw.rtcDevice);

	isInit = true;
}


void ls::lsEmbree::releaseEmbree()
{
	if (hw.rtcScene)
		rtcReleaseScene(hw.rtcScene);

	if (hw.rtcDevice)
		rtcReleaseDevice(hw.rtcDevice);
}


extern LRESULT CALLBACK wndGUIProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

ls::lsWnd::_wnd			ls::lsWnd::wnd;
ls::lsWnd::_hw			ls::lsWnd::hw;


bool ls::lsWnd ::lsWndInit(u32 w, u32 h, WNDPROC wndProc)
{
#if 0
	wnd.screenWidth = w;
	wnd.screenHeight = h;

	wnd.wndHinstance = GetModuleHandle(NULL);

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, wndProc, 0L, 0L, wnd.wndHinstance, NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("ImGui Example"), NULL };
	if (!RegisterClassEx(&wc))
	{
		std::cout << "Error: RegisterClassEx(&wc)" << std::endl;
		return false;
	}
	RECT R = { 0, 0, wnd.screenWidth, wnd.screenHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	wnd.wndHwnd = CreateWindowA("S Example", "JmxRender",
		WS_OVERLAPPEDWINDOW, 0, 0, width, height, 0, 0, wnd.wndHinstance, 0);

	if (!wnd.wndHwnd)
	{
		std::cout << "Error: CreateWindow()" << std::endl;
		return false;
	}


	ShowWindow(wnd.wndHwnd, SW_SHOW);
	UpdateWindow(wnd.wndHwnd);


	//Initiate DirectX11 and create SwapChain
	UINT createDeviceFlag = 0;
#if defined(DEBUG)|defined(_DEBUG)
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;

#endif // defined(DEBUG)|

	D3D_FEATURE_LEVEL feature;
	HR(D3D11CreateDevice(
		NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, createDeviceFlag,
		0, 0,
		D3D11_SDK_VERSION,
		&hw.d3dDevice,
		&feature,
		&hw.d3dImmediateContext));
	if (feature != D3D_FEATURE_LEVEL_11_0)
	{
		std::cout << "Error: fail to support directx11!" << std::endl;
		return false;
	}


	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferDesc.Width = wnd.screenWidth;
	sd.BufferDesc.Height = wnd.screenHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	sd.BufferCount = 1;
	sd.OutputWindow = wnd.wndHwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//Create DXGISWAPCHAIN
	//first,we need do it by IDXGIFactory
	//
	IDXGIDevice* dxgiDevice = 0;
	HR(hw.d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(hw.d3dDevice, &sd, &hw.swapChain));

	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	assert(hw.d3dDevice);
	assert(hw.d3dImmediateContext);
	assert(hw.swapChain);

	hw.renderTargetView->Release();
	hw.depthStencilView->Release();
	hw.depthStencilBuffer->Release();
	//ReleaseCom(mBackBuffer);


	HR(hw.swapChain->ResizeBuffers(
		1,
		wnd.screenWidth,
		wnd.screenHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		NULL
	));
	//	ID3D11Texture2D* mBackBuffer;
	HR(hw.swapChain->GetBuffer(0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&hw.backBuffer)));

	HR(hw.d3dDevice->CreateRenderTargetView(hw.backBuffer,
		NULL,
		&hw.renderTargetView));


	//	ReleaseCom(mBackBuffer);

	D3D11_TEXTURE2D_DESC td;
	td.Width = wnd.screenWidth;
	td.Height = wnd.screenHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = NULL;
	td.MiscFlags = NULL;

	HR(hw.d3dDevice->CreateTexture2D(
		&td,
		NULL,
		&hw.depthStencilBuffer));
	HR(hw.d3dDevice->CreateDepthStencilView(
		hw.depthStencilBuffer,
		NULL,
		&hw.depthStencilView));

	hw.d3dImmediateContext->OMSetRenderTargets(1, &hw.renderTargetView, hw.depthStencilView);

	hw.viewPort.TopLeftX = 0;
	hw.viewPort.TopLeftY = 0;
	hw.viewPort.Width = static_cast<float>(wnd.screenWidth);
	hw.viewPort.Height = static_cast<float>(wnd.screenHeight);
	hw.viewPort.MinDepth = 0.0f;
	hw.viewPort.MaxDepth = 1.0f;

	hw.d3dImmediateContext->RSSetViewports(1, &hw.viewPort);


#endif
	return true;

}

void ls::lsWnd::lsWndShutDown()
{
	hw.d3dDevice->Release();
	hw.d3dImmediateContext->Release();
	hw.swapChain->Release();
	hw.depthStencilBuffer->Release();
	hw.backBuffer->Release();
	hw.renderTargetView->Release();
	hw.depthStencilBuffer->Release();


}


ls::lsRender::_sampleInfo  ls::lsRender::sampleInfo;
ls::lsRender::_sceneRenderBlockInfo ls::lsRender::sceneRenderBlockInfo;