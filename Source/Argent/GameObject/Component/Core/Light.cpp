#include "Light.h"

void Light::Reset()
{
	color.Reset();
	ArComponent::Reset();
}
#ifdef _DEBUG
void Light::DrawDebug()
{
	if(ImGui::TreeNode(GetName().c_str()))
	{
		color.DrawDebug();
		ArComponent::DrawDebug();
		ImGui::TreePop();
	}
}
#endif
