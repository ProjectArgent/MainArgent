#include "ArEffect.h"

#include "ArResource.h"
#include "ArResourceManager.h"
#include "ArEffectManager.h"
#include "../Other/ArHelper.h"

namespace Argent::Resource::Effect
{
	ArEffect::ArEffect(const char* filePath):
		ArImportedResource(Argent::Resource::ArResourceManager::GenerateResourceUniqueId(), filePath, ResourceType::rEffect)
	{

	}

	void ArEffect::Play(const DirectX::XMFLOAT3& position, int32_t startFrame)
	{
		handle = ArEffectManager::Instance()->GetManager()->Play(effect, Helper::Effect::ToVector3D(position), startFrame);
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
