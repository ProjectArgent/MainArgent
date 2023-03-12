#include "ArEffectRenderer.h"
#include "Transform.h"
#include "../GameObject/GameObject.h"

namespace Argent::Component::Renderer
{
	ArEffectRenderer::ArEffectRenderer(const char* filePath, const char* materialPath):
		ArComponent(Helper::String::ExtractFileName(filePath, false))
	{
		effect = std::make_shared<Argent::Resource::Effect::ArEffect>(filePath, materialPath);
	}

	void ArEffectRenderer::Update()
	{
		if (!effect) return;

		//todo
		if (!effect->IsExist()) isPlay = false;
		isPlay = true;
	}

	void ArEffectRenderer::Render() const
	{
		if (!effect) return;
		const Transform* t = GetOwner()->GetTransform();

		if (!t) return;

		if(!effect->IsExist())
		effect->Play(t->GetPosition());
	}
}
