#include "yaMultiRenderTarget.h"
#include "yaTexture.h"

namespace md::graphics
{



	MultiRenderTarget::MultiRenderTarget()
		: mRenderTargets{}
		, mDSTexture(nullptr)
		, mRTCount(0)
		, mClearColors{}
	{
		int a = 0;
	}

	MultiRenderTarget::~MultiRenderTarget()
	{
	}

	void MultiRenderTarget::Create(std::shared_ptr<Texture> texture[8]
		, std::shared_ptr<Texture> dsTexture)
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (texture[i].get() == nullptr)
			{
				mRTCount = static_cast<UINT>(i);
				break;
			}

			mRenderTargets[i] = texture[i];
		}

		mDSTexture = dsTexture;
	}

	void MultiRenderTarget::OmSetRenderTarget()
	{
		//ID3D11RenderTargetView* view[8] = {};
		//GetDevice()->OMSetRenderTarget(8, view, nullptr);
		Texture::Clears();

		ID3D11RenderTargetView* arrRTV[8] = {};
		for (size_t i = 0; i < 8; i++)
		{
			if (mRenderTargets[i])
			{
				arrRTV[i] = mRenderTargets[i]->GetRTV().Get();
			}
		}

		if (mDSTexture.get() != nullptr)
		{
			GetDevice()->OMSetRenderTarget(mRTCount, arrRTV, mDSTexture->GetDSV().Get());
		}
		else
		{
			GetDevice()->OMSetRenderTarget(mRTCount, arrRTV, nullptr);
		}
	}

	void MultiRenderTarget::Clear(FLOAT backgroundColor[4])
	{
		//FLOAT backgroundColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

		for (size_t i = 0; i < mRTCount; i++)
		{
			if (mRenderTargets[i])
				GetDevice()->ClearRenderTargetView(mRenderTargets[i]->GetRTV().Get(), backgroundColor);
		}

		if (mDSTexture.get())
			GetDevice()->ClearDepthStencilView(mDSTexture->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	}

}
