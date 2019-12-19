#pragma once
#include<config/lsPtr.h>
#include<realtime/canvas.h>
#include<realtime/D3D11Realtime.h>
#include<math/transform.h>
namespace ls
{
	/*
		包含 RenderTargetView 和 ShaderResourceView 的 2D D3D11 画布
	*/
	class D3D11Canvas2D :public CanvasX<2, Vec4>
	{
	public:
		D3D11Canvas2D(s32 width, s32 height, Vec4* data = nullptr);
		virtual ~D3D11Canvas2D();


		virtual void* getHardwareResource() override {
			return mSRV;
		}
		
		virtual void drawQuad(s32 xStart, s32 xEnd, s32 yStart, s32 yEnd, void* data, CanvasOpAccessMode accessMode) override;
		





	protected:
		ID3D11RenderTargetView*		mRTV = nullptr;
		ID3D11ShaderResourceView*	mSRV = nullptr;
		ID3D11Texture2D*			mTex = nullptr;

		
	};
}