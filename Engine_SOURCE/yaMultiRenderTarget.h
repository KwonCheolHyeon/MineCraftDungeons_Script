#pragma once
#include "yaEntity.h"
#include "yaTexture.h"

namespace md::graphics
{
	class MultiRenderTarget : public Entity
	{
	public:
		MultiRenderTarget();
		~MultiRenderTarget();

		void Create(std::shared_ptr<Texture> texture[8], std::shared_ptr<Texture> dsTexture);
		void OmSetRenderTarget();
		void Clear(FLOAT backgroundColor[4]);

	private:
		std::shared_ptr<Texture> mRenderTargets[8];
		Vector4 mClearColors[8];
		std::shared_ptr<Texture> mDSTexture;
		UINT mRTCount;
	};
}

