#include "ArEffect.h"

#include "ArResource.h"
#include "ArResourceManager.h"
#include "ArEffectManager.h"
#include "../Other/ArHelper.h"

namespace Argent::Resource::Effect
{
	ArEffect::ArEffect(const char* filePath, const char* materialPath):
		ArImportedResource(Argent::Resource::ArResourceManager::GenerateResourceUniqueId(), filePath, ResourceType::rEffect)
	{
		std::wstring fPath = Helper::String::GetWideStringFromString(filePath);
		std::wstring mPath = Helper::String::GetWideStringFromString(materialPath);
		effect = Effekseer::Effect::Create(Argent::Resource::Effect::ArEffectManager::Instance()->GetManager(), (const EFK_CHAR*)(fPath.c_str()),
			1.0f, (const EFK_CHAR*)(mPath.c_str()));
		if (!effect.Get()) _ASSERT_EXPR(FALSE, L"failepath missing");

	}

	void ArEffect::Play(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale,
		const DirectX::XMFLOAT4& rotate, float startFrame)
	{
		handle = ArEffectManager::Instance()->GetManager()->Play(effect, Helper::Effect::ToVector3D(position), static_cast<int32_t>(startFrame));
	}

	void ArEffect::Update(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale,
		const DirectX::XMFLOAT4& rotate, const DirectX::XMFLOAT4& color)
	{
		ArEffectManager::Instance()->GetManager()->SetRotation(handle, rotate.x, rotate.y, rotate.z);
		ArEffectManager::Instance()->GetManager()->SetScale(handle, scale.x, scale.y, scale.z);
		ArEffectManager::Instance()->GetManager()->SetLocation(handle, position.x, position.y, position.z);

		Effekseer::Color col{static_cast<unsigned char>(color.x * 255),
			static_cast<unsigned char>(color.y * 255),
		static_cast<unsigned char>(color.z * 255),
		static_cast<unsigned char>(color.w * 255)};
		ArEffectManager::Instance()->GetManager()->SetAllColor(handle, col);
	}

	void ArEffect::Stop() const
	{
		ArEffectManager::Instance()->GetManager()->StopEffect(handle);
	}

	bool ArEffect::IsExist() const
	{
		return ArEffectManager::Instance()->GetManager()->Exists(handle);
	}
}
