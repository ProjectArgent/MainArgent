#pragma once
#include "ArRenderer.h"
#include "Data/Mesh.h"

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
		//ArMeshRenderer(Mesh* mesh);
		ArMeshRenderer(Argent::Data::ArMeshData* data);

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
		//Mesh* mesh;
		Argent::Data::ArMeshData* data;
	};
}
