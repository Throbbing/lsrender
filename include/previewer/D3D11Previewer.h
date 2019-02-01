#pragma once
#include<previewer/previewer.h>
#include<math/transform.h>
#include<function/file.h>
//Include for DirectX11 
#include<d3d11.h>
#include<DirectXMath.h>
#include<D3dcsx.h>
#include<d3dCompiler.h>
#include<3rd/DirectX/WICTextureLoader.h>
#include<3rd/DirectX/dxerr.h>
#include<3rd/DirectX/DirectXTex.h>
#include<map>






#define ReleaseCom(p) {if(p)p->Release();p=NULL;}
#define ErrorBox(text) {MessageBox(0, text, 0, 0);}


#define PAD16(x)		((x+15)&(~15))

using namespace DirectX;


namespace ls
{
	enum EShader
	{
		EVs,
		EGs,
		EHs,
		EDs,
		ECs,
		EPs,

	};
	void* createShaderAndLayout(LPCWSTR srcFile,
		const D3D10_SHADER_MACRO * macro,
		LPD3D10INCLUDE include,
		LPCSTR name, LPCSTR sm, EShader es,
		ID3D11Device* device,
		const D3D11_INPUT_ELEMENT_DESC* desc = nullptr,
		u32 eleNum = 0,
		ID3D11InputLayout** layout = nullptr);


	ID3D11Buffer* createConstantBuffer(u32 byteWidth, ID3D11Device* device,
		D3D11_USAGE usage = D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_FLAG cpuFlag = D3D11_CPU_ACCESS_WRITE
	);

	ID3D11ShaderResourceView* createSRV(ID3D11Resource* src, DXGI_FORMAT format,
		D3D11_SRV_DIMENSION dim,
		u32 arg0, u32 arg1, u32 arg2,
		ID3D11Device* device);

	ID3D11UnorderedAccessView* createUAV(ID3D11Resource* src, DXGI_FORMAT format,
		D3D11_UAV_DIMENSION dim,
		u32 arg0, u32 arg1, u32 arg2,
		ID3D11Device* device);

	ID3D11RenderTargetView* createRTV(ID3D11Resource* src, DXGI_FORMAT format,
		D3D11_RTV_DIMENSION dim,
		u32 arg0, u32 arg1, u32 arg2,
		ID3D11Device* device);

	ID3D11Buffer* createBuffer(D3D11_USAGE usage, u32 bindflag,
		u32 byteWidth,
		u32 misc,
		u32 cpuflag,
		u32 stride, void* data,
		ID3D11Device* device);

	ID3D11Texture2D* createTex2D(D3D11_USAGE usage, u32 bindflag,
		u32 width, u32 height, DXGI_FORMAT format,
		u32 miplevel, u32 arraysize,
		u32 misc, u32 cpuflag,
		u32 samCount, u32 samQuality,
		void* data,
		ID3D11Device* device);

	inline u32 sizeOF(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_UNKNOWN:
			return 0;
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 16;
		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 12;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return 8;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return 4;
		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
			return 2;
		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
			return 1;

		default:
			return 0;
		}
	}

	inline Mat4x4 XM2ls(XMFLOAT4X4 mat)
	{
		return Mat4x4(mat._11, mat._12, mat._13, mat._14,
			mat._21, mat._22, mat._23, mat._24,
			mat._31, mat._32, mat._33, mat._34,
			mat._41, mat._42, mat._43, mat._44);
	}
	inline Vec4 XM2ls(XMFLOAT4 vec)
	{
		return Vec4(vec.x, vec.y, vec.z, vec.w);
	}
	inline XMFLOAT4X4 ls2XM(Mat4x4 mat)
	{
		return XMFLOAT4X4(mat(0, 0), mat(0, 1), mat(0, 2), mat(0, 3),
			mat(1, 0), mat(1, 1), mat(1, 2), mat(1, 3),
			mat(2, 0), mat(2, 1), mat(2, 2), mat(2, 3),
			mat(3, 0), mat(3, 1), mat(3, 2), mat(3, 3));
	}
	inline XMFLOAT4 ls2XM(Vec4 vec)
	{
		return XMFLOAT4(vec.x, vec.y, vec.z, vec.w);
	}



	class SRVMgr
	{
	public:
		ID3D11ShaderResourceView* load(const Path& name,
			ID3D11Device* device)
		{
			if (srvs.find(name.str()) != srvs.end())
				return srvs[name.str()];

			ID3D11ShaderResourceView* srv;
			std::wstring wname = name.wstr();
			//			HR(D3DX11CreateShaderResourceViewFromFileA(device, name.c_str(),
			//				nullptr, nullptr, &srv, nullptr));

			ID3D11Resource* texR;
			HR(CreateWICTextureFromFile(device, wname.c_str(), &texR,
				&srv));
			ReleaseCom(texR);

			srvs[name.str()] = srv;

			return srvs[name.str()];
		}

		~SRVMgr()
		{
			for (auto& p : srvs)
				ReleaseCom(p.second);
		}

	private:
		std::map<std::string, ID3D11ShaderResourceView*>  srvs;
	};


	class D3D11Previewer :public Previewer
	{
	public:
		D3D11Previewer(u32 w, u32 h,
			HWND hwnd, HINSTANCE hinstance,
			ID3D11Device* device, ID3D11DeviceContext* context,
			ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv,
			const D3D11_VIEWPORT& vp);

		virtual ~D3D11Previewer();


		virtual void newFrame(f32 dt) override;
		virtual void render() override;

		virtual void commit() override;

	protected:
		void initGUI();
		void createFontsTexture();
		void createShader();
		void createBuf();
		void createStates();
		void render(ImDrawData* drawData);


		HWND							mWndHwnd;
		HINSTANCE					mWndHinstance;
		ID3D11Device*				md3dDevice;
		ID3D11DeviceContext*			md3dImmediateContext;
		ID3D11RenderTargetView*		mRTV;
		ID3D11DepthStencilView*		mDSV;


		ID3D11VertexShader*			mGuiVs;
		ID3D11PixelShader*			mGuiPs;
		ID3D11Buffer*				mCBTrans;
		ID3D11Buffer*				mVB;
		u32							mVerticesCount;
		ID3D11Buffer*				mIB;
		u32							mIndicesCount;
		ID3D11InputLayout*			mLayout;
		ID3D11SamplerState*			mSam;
		ID3D11ShaderResourceView*	mFontSRV;

		ID3D11BlendState*			mBS;
		ID3D11DepthStencilState*	mDSS;
		ID3D11RasterizerState*		mRS;
		D3D11_VIEWPORT				mViewPort;

		u32							mWidth;
		u32							mHeight;



		ID3D11Buffer*				mPoint3DVB = nullptr;
		ID3D11Buffer*				mLine3DVB = nullptr;
		ID3D11Buffer*				mTri3DVB = nullptr;
		ID3D11Buffer*				mQuadTexVB = nullptr;
	};




}


