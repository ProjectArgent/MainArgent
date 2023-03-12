#include "EffectRenderer.h"
#include "Graphic/ArGraphics.h"
#include "Other/Misc.h"
#include "Core/ArTimer.h"

namespace Argent::Effect
{
	EffectRenderer::EffectRenderer()
	{
		//なんか戻り値がリファレンスポインタだったので魔導書通りにやるためにgetを使ってポインタにしている
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		efkRenderer = EffekseerRendererDX12::Create(
			Graphics::ArGraphics::Instance()->GetDevice(),
			Graphics::ArGraphics::Instance()->GetCommandQueue(),
			Graphics::ArGraphics::GetNumBackBuffers(),
			&format,
			1,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			false,
			10000
		);

		 if (!efkRenderer.Get()) _ASSERT_EXPR(FALSE, "Error");

		efkManager = Effekseer::Manager::Create(10000);
		if(!efkManager.Get()) _ASSERT_EXPR(FALSE, "Error");

		//座標系のセット
		efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

		//描画機能のセット 自作もできるらしいがそのまま使う
		efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
		efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
		efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
		efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
		efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

		//ローダーのセット　こちらも自作可能
		efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
		efkManager->SetModelLoader(efkRenderer->CreateModelLoader());

		
		//dx12特有の処理

		efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());
		if (!efkMemoryPool.Get()) _ASSERT_EXPR(FALSE, "Error");
		efkCmdList = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);
		if(!efkCmdList.Get()) _ASSERT_EXPR(FALSE, "Error");

		efkRenderer->SetCommandList(efkCmdList);


		effect = Effekseer::Effect::Create(efkManager, (const EFK_CHAR*)L"./Resources/Effect/Effects/10/SimpleLaser.efk",
			1.0f, (const EFK_CHAR*)L"./Resources/Effect/Effects/10");

		
	}

	void EffectRenderer::Update()
	{
		Effekseer::Matrix44 fkViewMat;
		Effekseer::Matrix44 fkProjMat;

		const auto view = Graphics::ArGraphics::Instance()->GetCurrentFrameResource()->GetSceneView();
		const auto projection = Graphics::ArGraphics::Instance()->GetCurrentFrameResource()->GetSceneProjection();

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				fkViewMat.Values[i][j] = view.m[i][j];
				fkProjMat.Values[i][j] = projection.m[i][j];
			}
		}
		efkRenderer->SetCameraMatrix(fkViewMat);
		efkRenderer->SetProjectionMatrix(fkProjMat);

		static float elapsedTime{};
		elapsedTime += Argent::Timer::ArTimer::Instance().DeltaTime();
		if(elapsedTime > 3.0f)
		{
			elapsedTime = 0;
			handle = efkManager->Play(effect, 0, 0, 0);
		}

		efkManager->Update();

		//描画場所を指定　マルチレンダーターゲット場合は一枚目でやること
		efkMemoryPool->NewFrame();
		EffekseerRendererDX12::BeginCommandList(efkCmdList, Graphics::ArGraphics::Instance()->GetCommandList());

		efkRenderer->BeginRendering();
		efkManager->Draw();
		efkRenderer->EndRendering();


		EffekseerRendererDX12::EndCommandList(efkCmdList);
	}
}
