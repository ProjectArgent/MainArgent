#include "Transform.h"

#include "../GameObject/GameObject.h"
#include "Camera.h"
#include "Light.h"

Transform& Transform::operator+=(const Transform& t)
{
	this->position.x += t.position.x;
	this->position.y += t.position.y;
	this->position.z += t.position.z;

	this->rotation.x += t.rotation.x;
	this->rotation.y += t.rotation.y;
	this->rotation.z += t.rotation.z;
	return *this;
}
#ifdef _DEBUG
void Transform::DrawDebug()
{
	if(ImGui::TreeNode(GetName().c_str()))
	{
		ImGui::DragFloat3("Position", &position.x, 1.0f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("Scale", &scale.x, 0.001f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("Rotation", &rotation.x, 0.5f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat("ScaleFactor", &scaleFactor, 0.001f, 0.001f, 1.0f);
		ImGui::SliderInt("Coordinate System", &coordinateSystem, 0, static_cast<int>(CoordinateSystem::cLeftZup));

		ImGui::Text(coordinateSystemName[coordinateSystem].c_str());


		ArComponent::DrawDebug();
		ImGui::TreePop();
	}

}
#endif
void Transform::Reset()
{
	if(GetOwner()->GetComponent<Camera>() || GetOwner()->GetComponent<Light>())
	{
		position = DirectX::XMFLOAT3(0, 0, -10);
	}
	else
	{
		position = DirectX::XMFLOAT3();
	}
	scale = DirectX::XMFLOAT3(1, 1, 1);
	rotation = DirectX::XMFLOAT4(0, 0, 0, 0);
}

Transform Transform::AdjustParentTransform() const
{
	Transform tmp{};
	tmp += *this;
	if(GetOwner())
	{
		if(GetOwner()->GetParent())
		{
			tmp += GetOwner()->GetParent()->GetTransform()->AdjustParentTransform();
		}
	}
	return tmp;
}
