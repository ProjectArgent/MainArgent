#pragma once
#include <memory>
#include "../ArComponent.h"
#include "Data/ArMaterial.h"
#include "Data/Mesh.h"
#include "../../../Graphic/Dx12/PipelineState.h"

class Renderer:
	public Argent::Component::ArComponent
{
public:
	Renderer(std::string name):
		ArComponent(name)
	{
		material = std::make_unique<Argent::Material::ArMaterial>(L"");
	}

	virtual void Reset() override
	{
	
	}

#ifdef _DEBUG
	void DrawDebug() override
	{
		
		Argent::Component::ArComponent::DrawDebug();
		}
#endif
protected:
	std::unique_ptr<Argent::Material::ArMaterial> material;
};


class MeshRenderer:
	public Renderer
{
public:
	MeshRenderer(Mesh* mesh);
	~MeshRenderer() override = default;

	void Render() override;
#ifdef _DEBUG
	void DrawDebug() override;
#endif

private:
	Mesh* mesh;
	std::unique_ptr<Argent::Dx12::ArRenderingPipeline> renderingPipeline;
	std::unique_ptr<Argent::Material::ArMaterial> material;
};
