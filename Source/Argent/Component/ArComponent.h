#pragma once
#include <DirectXMath.h>
#include <string>

#include "../imgui/imgui.h"
class GameObject;


//コンポーネントクラスの大元
namespace Argent::Component
{
	class ArComponent
	{
	public:
		ArComponent(const std::string& n) :
			owner(nullptr)
		{
			name = n;
		}
		virtual  ~ArComponent() = default;
		ArComponent(const ArComponent& com) = default;
		ArComponent(const ArComponent&& com) noexcept;

		GameObject* GetOwner() const { return owner; }
		void SetOwner(GameObject* p) { owner = p; }

		virtual void Initialize() { Reset(); }	//最初に一回だけ呼ばれる
		virtual void Finalize() {}				//最後に一回だけ呼ばれる
		virtual void Begin() {}					//毎フレーム最初に呼ばれる
		virtual void End() {}					//毎フレーム最後に呼ばれる
		virtual void Update() {}				//毎フレームBeginとEndの間で呼ばれる
		virtual void Render() {}				//毎フレーム　描画用関数
		virtual void Reset() {}					//Initializeが呼ばれた直後の状態に戻す
#ifdef _DEBUG
		virtual void DrawDebug()
		{
			DebugReset();
		}

		void DebugReset()
		{
			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}
#endif

	protected:
		std::string GetName() const { return name; }

	private:
		GameObject* owner;
		std::string name;
		//std::vector<std::unordered_map<char[], std::set<char>>>* unti;
	};

}

struct Color
{
	Color(DirectX::XMFLOAT4 color):
		color(color)
	{}

	void Reset() { color = DirectX::XMFLOAT4(1, 1, 1, 1); }

#ifdef _DEBUG
	void DrawDebug()
	{
		ImGui::ColorEdit4("Color", &color.x,
				ImGuiColorEditFlags_PickerHueWheel | 
				ImGuiColorEditFlags_AlphaBar);
	}
#endif

	[[nodiscard]] DirectX::XMFLOAT4 GetColor() const { return color; }
	void SetColor(const DirectX::XMFLOAT4& c) { color = c; }

private:
	DirectX::XMFLOAT4 color;
};
