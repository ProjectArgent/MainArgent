#pragma once
#include "ArRenderer.h"

namespace Argent
{
	namespace Data
	{
		class ArMesh;
	}
}

namespace Argent::Component::Renderer
{
	struct Subset
	{
		uint32_t startVertex{};
		uint32_t vertexCount{};
		uint32_t startIndex{};
		uint32_t indexCount{};
	};


	class ArMeshRenderer :
		public ArRenderer
	{
	public:
		ArMeshRenderer(Argent::Data::ArMesh* data);

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
		
		std::vector<Subset> subsets;
		Argent::Data::ArMesh* mesh;
	};
}
