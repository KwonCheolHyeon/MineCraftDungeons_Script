#pragma once
#include "mdUIBase.h"
#include "UIEnums.h"

namespace md
{
	class UIAlphaObject :
		public UIBase
	{
	public:
		UIAlphaObject();
		virtual ~UIAlphaObject();

		void Initalize() override;
		void Update() override;
		void FixedUpdate() override;
		void Render() override;


	public:
		void SetHeartFrameAlpha(float _alpha) { mHeartAlpha = _alpha; }
		void SetAlphaTimeZero() { mTime = 0.f; }
		void CloseAlpha();//알파값을 점점 어둡게 함

	private://무슨 타입 인가?
		int mType;

	private: // 하트 관련
		float mHeartAlpha;
	private://선택 관련
		bool bEnable;

	private://HP 시간 관련
		float mTime;
		float mHeartAlphaTimer;

	private://최종 적으로 보낼 alpha값
		float mObjectAlpha;
		eUIAlphaObjectState mAOState;

	};
}


