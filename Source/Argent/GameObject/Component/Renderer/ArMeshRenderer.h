#pragma once
#include "ArRenderer.h"

namespace Argent
{
	namespace Data
	{
		class ArMeshData;
	}
}

namespace Argent::Component::Renderer
{
	class ArMeshRenderer :
		public ArRenderer
	{
	public:
		ArMeshRenderer();

		void Initialize() override;
		void Finalize() override;
		void Begin() override;
		void End() override;
		void Update() override;
		void Render() override;

#ifdef _DEBUG
		void DrawDebug() override;
#endif

	protected:
		std::unique_ptr<Data::ArMeshData> data;
	};
}
