#include<previewer/D3D11Previewer.h>

LRESULT CALLBACK wndGUIProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		ld = true;

		gLastMouseX = GET_X_LPARAM(lParam);
		gLastMouseY = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		ld = false;
		ReleaseCapture();
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		//		std::cout << io.MousePos.x << "  " << io.MousePos.y << std::endl;
		if (((wParam & MK_LBUTTON) != 0))
			gMouseMove = true;
		gMouseX = GET_X_LPARAM(lParam);
		gMouseY = GET_Y_LPARAM(lParam);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}


void* ls::createShaderAndLayout(LPCWSTR srcFile,
	const D3D10_SHADER_MACRO * macro,
	LPD3D10INCLUDE include,
	LPCSTR name, LPCSTR sm, EShader es,
	ID3D11Device* device,
	const D3D11_INPUT_ELEMENT_DESC* desc,
	u32 descNum,
	ID3D11InputLayout** layout)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* blob = nullptr;
	ID3DBlob* err = nullptr;
	HR(D3DCompileFromFile(srcFile, macro, include, name, sm, dwShaderFlags,
		0, &blob, &err));


	if (err)
	{

		char* msg = (char*)err->GetBufferPointer();
		std::cout << msg << std::endl;
		ReleaseCom(err);
		//		system("pause");

	}

	if (desc&&descNum > 0)
	{
		HR(device->CreateInputLayout(desc, descNum,
			blob->GetBufferPointer(), blob->GetBufferSize(), layout));

	}

	switch (es)
	{
	case ls::EVs:
		ID3D11VertexShader* vs;
		HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vs));
		return (void*)vs;
		break;
	case ls::EGs:
		ID3D11GeometryShader* gs;
		HR(device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &gs));
		return (void*)gs;
		break;
	case ls::EHs:
		ID3D11HullShader* hs;
		HR(device->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &hs));
		return (void*)hs;
		break;
	case ls::EDs:
		ID3D11DomainShader* ds;
		HR(device->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &ds));
		return (void*)ds;
		break;
	case ls::ECs:
		ID3D11ComputeShader* cs;
		HR(device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &cs));
		return (void*)cs;
		break;
	case ls::EPs:
		ID3D11PixelShader* ps;
		HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &ps));
		return (void*)ps;
		break;
	default:return nullptr;
		break;
	}

}

ID3D11Buffer* ls::createConstantBuffer(u32 byteWidth, ID3D11Device* device,
	D3D11_USAGE usage /* = D3D11_USAGE_DYNAMIC */,
	D3D11_CPU_ACCESS_FLAG cpuFlag /* = D3D11_CPU_ACCESS_WRITE */)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = byteWidth;
	bd.CPUAccessFlags = cpuFlag;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	ID3D11Buffer* buf;
	HR(device->CreateBuffer(&bd, 0, &buf));

	return buf;
}




ID3D11ShaderResourceView* ls::createSRV(ID3D11Resource* src, DXGI_FORMAT format,
	D3D11_SRV_DIMENSION dim,
	u32 arg0, u32 arg1, u32 arg2,
	ID3D11Device* device)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = dim;

	switch (dim)
	{
	case D3D11_SRV_DIMENSION_BUFFER:
		srvd.Buffer.FirstElement = 0;
		srvd.Buffer.NumElements = arg0;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE2D:
		srvd.Texture2D.MipLevels = arg0;
		srvd.Texture2D.MostDetailedMip = arg1;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
		srvd.Texture2DArray.MostDetailedMip = 0;
		srvd.Texture2DArray.ArraySize = arg0;
		srvd.Texture2DArray.FirstArraySlice = arg1;
		srvd.Texture2DArray.MipLevels = arg2;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE3D:
		srvd.Texture3D.MipLevels = arg0;
		srvd.Texture3D.MostDetailedMip = arg1;
		break;
	case D3D11_SRV_DIMENSION_BUFFEREX:
		srvd.BufferEx.FirstElement = 0;
		srvd.BufferEx.NumElements = arg0;
		srvd.BufferEx.Flags = arg1;
		break;
	default:return nullptr;
		break;
	}
	ID3D11ShaderResourceView* srv;
	HR(device->CreateShaderResourceView(src, &srvd, &srv));

	return srv;


}

ID3D11UnorderedAccessView* ls::createUAV(ID3D11Resource* src,
	DXGI_FORMAT format, D3D11_UAV_DIMENSION dim,
	u32 arg0, u32 arg1, u32 arg2,
	ID3D11Device* device)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	ZeroMemory(&uavd, sizeof(uavd));
	uavd.Format = format;
	uavd.ViewDimension = dim;
	switch (dim)
	{

	case D3D11_UAV_DIMENSION_BUFFER:
		uavd.Buffer.FirstElement = 0;
		uavd.Buffer.NumElements = arg0;
		uavd.Buffer.Flags = arg1;
		break;
	case D3D11_UAV_DIMENSION_TEXTURE2D:
		uavd.Texture2D.MipSlice = 0;
		break;
	case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
		uavd.Texture2DArray.MipSlice = arg0;
		uavd.Texture2DArray.FirstArraySlice = arg1;
		uavd.Texture2DArray.ArraySize = arg2;
		break;
	case D3D11_UAV_DIMENSION_TEXTURE3D:
		uavd.Texture3D.MipSlice = 0;
		uavd.Texture3D.FirstWSlice = 0;
		uavd.Texture3D.WSize = arg0;
		break;
	default:return nullptr;
		break;
	}

	ID3D11UnorderedAccessView* uav;
	HR(device->CreateUnorderedAccessView(src, &uavd, &uav));

	return uav;
}

ID3D11RenderTargetView* ls::createRTV(ID3D11Resource* src,
	DXGI_FORMAT format, D3D11_RTV_DIMENSION dim,
	u32 arg0, u32 arg1, u32 arg2,
	ID3D11Device* device)
{
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = dim;
	switch (dim)
	{
	case D3D11_RTV_DIMENSION_TEXTURE1D:
		rtvd.Texture1D.MipSlice = arg0;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
		rtvd.Texture1DArray.MipSlice = arg0;
		rtvd.Texture1DArray.FirstArraySlice = arg1;
		rtvd.Texture1DArray.ArraySize = arg2;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2D:
		rtvd.Texture2D.MipSlice = arg0;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
		rtvd.Texture2DArray.MipSlice = arg0;
		rtvd.Texture2DArray.FirstArraySlice = arg1;
		rtvd.Texture2DArray.ArraySize = arg2;
		break;
	case D3D11_RTV_DIMENSION_TEXTURE3D:
		rtvd.Texture3D.MipSlice = arg0;
		rtvd.Texture3D.FirstWSlice = arg1;
		rtvd.Texture3D.WSize = arg2;
		break;
	default:
		return nullptr;
		break;
	}
	ID3D11RenderTargetView * rtv;
	HR(device->CreateRenderTargetView(src, &rtvd, &rtv));

	return rtv;
}

ID3D11Buffer* ls::createBuffer(D3D11_USAGE usage, u32 bindflag,
	u32 byteWidth,
	u32 misc,
	u32 cpuflag,
	u32 stride, void* data,
	ID3D11Device* device)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = usage;
	bd.BindFlags = bindflag;
	bd.ByteWidth = byteWidth;
	bd.MiscFlags = misc;
	bd.CPUAccessFlags = cpuflag;
	bd.StructureByteStride = stride;

	ID3D11Buffer* buf = nullptr;
	D3D11_SUBRESOURCE_DATA sd;
	if (data)
	{
		sd.pSysMem = data;
		HR(device->CreateBuffer(&bd, &sd, &buf));
	}
	else
	{
		HR(device->CreateBuffer(&bd, 0, &buf));
	}

	return buf;

}

ID3D11Texture2D* ls::createTex2D(D3D11_USAGE usage, u32 bindflag,
	u32 width, u32 height, DXGI_FORMAT format,
	u32 miplevel, u32 arraysize,
	u32 misc, u32 cpuflag,
	u32 samCount, u32 samQuality,
	void* data,
	ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Usage = usage;
	td.BindFlags = bindflag;
	td.Width = width;
	td.Height = height;
	td.Format = format;
	td.MipLevels = miplevel;
	td.ArraySize = arraysize;
	td.MiscFlags = misc;
	td.CPUAccessFlags = cpuflag;
	td.SampleDesc.Count = samCount;
	td.SampleDesc.Quality = samQuality;

	ID3D11Texture2D* tex;
	if (data)
	{
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem = data;
		sd.SysMemPitch = width*sizeOF(format);
		HR(device->CreateTexture2D(&td, &sd, &tex));
	}
	else
	{
		HR(device->CreateTexture2D(&td, nullptr, &tex));
	}

	return tex;




}

ls::D3D11Previewer::D3D11Previewer(u32 w, u32 h, HWND hwnd, HINSTANCE hinstance, ID3D11Device * device, ID3D11DeviceContext * context, ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, const D3D11_VIEWPORT & vp)
{
	mWidth = w;
	mHeight = h;
	mWndHwnd = hwnd;
	mWndHinstance = hinstance;
	md3dDevice = device;
	md3dImmediateContext = context;
	mRTV = rtv;
	mDSV = dsv;
	mViewPort = vp;

	initGUI();
	createFontsTexture();
	createShader();
	createBuf();
	createStates();
}

ls::D3D11Previewer::~D3D11Previewer()
{
	ReleaseCom(mGuiVs);
	ReleaseCom(mGuiPs);
	ReleaseCom(mCBTrans);
	ReleaseCom(mVB);
	ReleaseCom(mIB);
	ReleaseCom(mLayout);
	ReleaseCom(mSam);
	ReleaseCom(mFontSRV);
	ReleaseCom(mBS);
	ReleaseCom(mDSS);
	ReleaseCom(mRS);
}

void ls::D3D11Previewer::newFrame(f32 dt)
{
	ImGuiIO& io = ImGui::GetIO();
	static f32 totalTime = 0.f;
	static u32 frames = 0;
	RECT rect;
	GetClientRect(mWndHwnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	// Setup time step

	io.DeltaTime = deltaTime;
	totalTime += deltaTime;
	frames++;
	if (totalTime > 1.f)
	{

		jmxREX::wnd.fps = (f32)frames / totalTime;
		totalTime = 0.f;
		frames = 0;
	}

	// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;
	// io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
	// io.MousePos : filled by WM_MOUSEMOVE events
	// io.MouseDown : filled by WM_*BUTTON* events
	// io.MouseWheel : filled by WM_MOUSEWHEEL events

	// Hide OS mouse cursor if ImGui is drawing it
	if (io.MouseDrawCursor)
		SetCursor(NULL);

	// Start the frame
	ImGui::NewFrame();
}

void ls::D3D11Previewer::render()
{

}

void ls::D3D11Previewer::commit()
{
	std::lock_guard<std::mutex> lck(mMutex);

	if (!mHasChanged)
		return;

	
	mHasChanged = false;
	if (mPoint3DVB)
		ReleaseCom(mPoint3DVB);
	if (mLine3DVB)
		ReleaseCom(mLine3DVB);
	if (mTri3DVB)
		ReleaseCom(mTri3DVB);
	if (mQuadTexVB)
		ReleaseCom(mQuadTexVB);

	if(!mPoint3DData.empty())
		mPoint3DVB = createBuffer(D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			mPoint3DData.size() * sizeof(PrePoint),
			0,0,0,
			&mPoint3DData[0],
			lsWnd::hw.d3dDevice);

	if(!mLine3DData.empty())
		mLine3DVB = createBuffer(D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			mLine3DData.size() * sizeof(PreLine),
			0, 0, 0,
			&mLine3DData[0],
			lsWnd::hw.d3dDevice);

	if (!mTri3DData.empty())
		mTri3DVB = createBuffer(D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			mTri3DData.size() * sizeof(PreTri),
			0, 0, 0,
			&mTri3DData[0],
			lsWnd::hw.d3dDevice);

}


void ls::D3D11Previewer::initGUI()
{


	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.RenderDrawListsFn = std::bind(&D3D11Previewer::render, std::placeholders::_1);  // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.ImeWindowHandle = mWndHwnd;
}


void ls::D3D11Previewer::createFontsTexture()
{
	ImGuiIO& io = ImGui::GetIO();
	u8* pixels = nullptr;
	s32 width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Upload texture to graphics system
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D *pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = pixels;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		md3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		md3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &mFontSRV);
		pTexture->Release();
	}

	// Store our identifier
	io.Fonts->TexID = (void *)mFontSRV;

	// Create texture sampler
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.f;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		md3dDevice->CreateSamplerState(&desc, &mSam);
	}

}

void ls::D3D11Previewer::createShader()
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((ImDrawVert*)0)->uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	mGuiVs = (ID3D11VertexShader*)createShaderAndLayout(L"HLSL/EditorVS.hlsl", nullptr, nullptr, "main", "vs_5_0",
		EVs, md3dDevice, layout, 3, &mLayout);

	mGuiPs = (ID3D11PixelShader*)createShaderAndLayout(L"HLSL/EditorPS.hlsl", nullptr, nullptr, "main", "ps_5_0",
		EPs, md3dDevice);
}

void ls::D3D11Previewer::createBuf()
{
	mCBTrans = createConstantBuffer(PAD16(sizeof(XMFLOAT4X4)), md3dDevice);
}

void ls::D3D11Previewer::createStates()
{
	{
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		md3dDevice->CreateBlendState(&desc, &mBS);
	}

	// Create the rasterizer state
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.ScissorEnable = true;
		desc.DepthClipEnable = true;
		md3dDevice->CreateRasterizerState(&desc, &mRS);
	}

	// Create depth-stencil State
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;
		md3dDevice->CreateDepthStencilState(&desc, &mDSS);
	}
}

void ls::D3D11Previewer::render(ImDrawData* drawData)
{
	if (!mVB || mVerticesCount < drawData->TotalVtxCount)
	{
		if (mVB)
			ReleaseCom(mVB);

		mVerticesCount = drawData->TotalVtxCount + 5000;
		mVB = createBuffer(D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			mVerticesCount * sizeof(ImDrawVert),
			0, D3D11_CPU_ACCESS_WRITE, 0, nullptr, md3dDevice);
	}

	if (!mIB || mIndicesCount < drawData->TotalVtxCount)
	{
		if (mIB)
			ReleaseCom(mIB);

		mIndicesCount = drawData->TotalVtxCount + 10000;

		mIB = createBuffer(D3D11_USAGE_DYNAMIC,
			D3D11_BIND_INDEX_BUFFER,
			mIndicesCount * sizeof(ImDrawIdx),
			0, D3D11_CPU_ACCESS_WRITE, 0, nullptr, md3dDevice);
	}

	D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	if (md3dImmediateContext->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
		return;
	if (md3dImmediateContext->Map(mIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
		return;
	ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
	ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	md3dImmediateContext->Unmap(mVB, 0);
	md3dImmediateContext->Unmap(mIB, 0);

	// Setup orthographic projection matrix into our constant buffer
	{
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		if (md3dImmediateContext->Map(mCBTrans, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
			return;
		XMFLOAT4X4* constant_buffer = (XMFLOAT4X4*)mapped_resource.pData;
		float L = 0.0f;
		float R = ImGui::GetIO().DisplaySize.x;
		float B = ImGui::GetIO().DisplaySize.y;
		float T = 0.0f;
		XMMATRIX mvp = XMMATRIX(2.0f / (R - L), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (T - B), 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f);

		XMStoreFloat4x4(constant_buffer, XMMatrixTranspose(mvp));
		md3dImmediateContext->Unmap(mCBTrans, 0);
	}

	// Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
	struct BACKUP_DX11_STATE
	{
		UINT                        ScissorRectsCount, ViewportsCount;
		D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		ID3D11RasterizerState*      RS;
		ID3D11BlendState*           BlendState;
		FLOAT                       BlendFactor[4];
		UINT                        SampleMask;
		UINT                        StencilRef;
		ID3D11DepthStencilState*    DepthStencilState;
		ID3D11ShaderResourceView*   PSShaderResource;
		ID3D11SamplerState*         PSSampler;
		ID3D11PixelShader*          PS;
		ID3D11VertexShader*         VS;
		UINT                        PSInstancesCount, VSInstancesCount;
		ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
		D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
		ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
		UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
		DXGI_FORMAT                 IndexBufferFormat;
		ID3D11InputLayout*          InputLayout;
	};
	BACKUP_DX11_STATE old;
	old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	md3dImmediateContext->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
	md3dImmediateContext->RSGetViewports(&old.ViewportsCount, old.Viewports);
	md3dImmediateContext->RSGetState(&old.RS);
	md3dImmediateContext->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
	md3dImmediateContext->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
	md3dImmediateContext->PSGetShaderResources(0, 1, &old.PSShaderResource);
	md3dImmediateContext->PSGetSamplers(0, 1, &old.PSSampler);
	old.PSInstancesCount = old.VSInstancesCount = 256;
	md3dImmediateContext->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
	md3dImmediateContext->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
	md3dImmediateContext->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
	md3dImmediateContext->IAGetPrimitiveTopology(&old.PrimitiveTopology);
	md3dImmediateContext->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
	md3dImmediateContext->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
	md3dImmediateContext->IAGetInputLayout(&old.InputLayout);

	// Setup viewport
	D3D11_VIEWPORT vp;
	memset(&vp, 0, sizeof(D3D11_VIEWPORT));
	vp.Width = ImGui::GetIO().DisplaySize.x;
	vp.Height = ImGui::GetIO().DisplaySize.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	md3dImmediateContext->RSSetViewports(1, &vp);

	// Bind shader and vertex buffers
	unsigned int stride = sizeof(ImDrawVert);
	unsigned int offset = 0;
	md3dImmediateContext->IASetInputLayout(mLayout);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIB, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->VSSetShader(mGuiVs, NULL, 0);
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &mCBTrans);
	md3dImmediateContext->PSSetShader(mGuiPs, NULL, 0);
	md3dImmediateContext->PSSetSamplers(0, 1, &mSam);

	// Setup render state
	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	md3dImmediateContext->OMSetBlendState(mBS, blend_factor, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(mDSS, 0);
	md3dImmediateContext->RSSetState(mRS);

	// Render command lists
	int vtx_offset = 0;
	int idx_offset = 0;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				const D3D11_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
				md3dImmediateContext->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&pcmd->TextureId);
				md3dImmediateContext->RSSetScissorRects(1, &r);
				md3dImmediateContext->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}

	// Restore modified DX state
	md3dImmediateContext->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
	md3dImmediateContext->RSSetViewports(old.ViewportsCount, old.Viewports);
	md3dImmediateContext->RSSetState(old.RS); if (old.RS) old.RS->Release();
	md3dImmediateContext->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask); if (old.BlendState) old.BlendState->Release();
	md3dImmediateContext->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef); if (old.DepthStencilState) old.DepthStencilState->Release();
	md3dImmediateContext->PSSetShaderResources(0, 1, &old.PSShaderResource); if (old.PSShaderResource) old.PSShaderResource->Release();
	md3dImmediateContext->PSSetSamplers(0, 1, &old.PSSampler); if (old.PSSampler) old.PSSampler->Release();
	md3dImmediateContext->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount); if (old.PS) old.PS->Release();
	for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
	md3dImmediateContext->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); if (old.VS) old.VS->Release();
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer); if (old.VSConstantBuffer) old.VSConstantBuffer->Release();
	for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
	md3dImmediateContext->IASetPrimitiveTopology(old.PrimitiveTopology);
	md3dImmediateContext->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset); if (old.IndexBuffer) old.IndexBuffer->Release();
	md3dImmediateContext->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
	md3dImmediateContext->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();
}

