#include<config/common.h>
#include<realtime/D3D11Canvas.h>

ls::D3D11Canvas2D::D3D11Canvas2D(s32 width, s32 height, Vec4 * data):
	CanvasX<2, Vec4>(std::array<s32,2>{width,height},data)
{
	
	mTex = createTex2D(D3D11_USAGE_DYNAMIC,
		D3D11_BIND_SHADER_RESOURCE ,
		width,
		height,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		1, 1, 0,
		D3D11_CPU_ACCESS_WRITE,
		1, 0,
		data,
		D3D11Renderer::hw.d3dDevice);

	mSRV = createSRV(mTex,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		1, 0, 0,
		D3D11Renderer::hw.d3dDevice);

	mRTV = createRTV(mTex,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D11_RTV_DIMENSION_TEXTURE2D,
		0, 0, 0,
		D3D11Renderer::hw.d3dDevice);

}

ls::D3D11Canvas2D::~D3D11Canvas2D()
{
	ReleaseCom(mTex);
	ReleaseCom(mSRV);
	ReleaseCom(mRTV);
}

void ls::D3D11Canvas2D::drawQuad(s32 xStart, s32 xEnd, s32 yStart, s32 yEnd, void * data, CanvasOpAccessMode accessMode)
{
	ls_Assert(mTex && mSRV && mRTV && data);

	Vec4* pData = (Vec4*)(data);
	s32 width = mResolution[0];
	s32 height = mResolution[1];

	if (accessMode == ECanvasOpAccess_CPU)
	{
		for (s32 y = yStart; y < yEnd; ++y)
		{
			for (s32 x = xStart; x < xEnd; ++x)
			{
				mData[y*width + x] = pData[(y - yStart) * (xEnd - xStart) + (x - xStart)];
			}
		}

		auto context = D3D11Renderer::hw.d3dImmediateContext;

		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(ms));
		context->Map(mTex, 0, D3D11_MAP_WRITE_DISCARD, 0,
			&ms);
		Vec4* p = (Vec4*)ms.pData;
		std::memcpy(p, mData, sizeof(Vec4)* width * height);
		context->Unmap(mTex, 0);
	}
}
