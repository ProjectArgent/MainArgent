#include "ArEffectRenderer.h"
#include "Transform.h"
#include "../GameObject/GameObject.h"

namespace Argent::Component::Renderer
{
	void ArEffectRenderer::Update()
	{
		if (!effect) return;

		if (!effect->IsExist()) isPlay = false;
	}

	void ArEffectRenderer::Render() const
	{
		if (!effect) return;
		const Transform* t = GetOwner()->GetTransform();

		if (!t) return;

		effect->Play(t->GetPosition());
	}
}
