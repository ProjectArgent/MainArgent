#include "ArResourceManager.h"


namespace Argent::Resource
{
	namespace Primitive
	{
		std::unique_ptr<MeshData> CreateCube()
		{
			constexpr float width = 0.5f, height = 0.5f, depth = 0.5f;
			std::vector<Argent::Data::ArMeshData::Vertex> vertices
			{
				{{-width,  height, -depth}, {-1,  0,  0} },
				{{-width,  height, -depth}, { 0,  1,  0} },
				{{-width,  height, -depth}, { 0,  0, -1} },
				{{-width,  height,  depth}, {-1,  0,  0} },
				{{-width,  height,  depth}, { 0,  1,  0} },
				{{-width,  height,  depth}, { 0,  0,  1} },
				{{ width,  height,  depth}, { 1,  0,  0} },
				{{ width,  height,  depth}, { 0,  1,  0} },
				{{ width,  height,  depth}, { 0,  0,  1} },
				{{ width,  height, -depth}, { 1,  0,  0} },
				{{ width,  height, -depth}, { 0,  1,  0} },
				{{ width,  height, -depth}, { 0,  0, -1} },
				{{-width, -height, -depth}, {-1,  0,  0} },
				{{-width, -height, -depth}, { 0, -1,  0} },
				{{-width, -height, -depth}, { 0,  0, -1} },
				{{-width, -height,  depth}, {-1,  0,  0} },
				{{-width, -height,  depth}, { 0, -1,  0} },
				{{-width, -height,  depth}, { 0,  0,  1} },
				{{ width, -height,  depth}, { 1,  0,  0} },
				{{ width, -height,  depth}, { 0, -1,  0} },
				{{ width, -height,  depth}, { 0,  0,  1} },
				{{ width, -height, -depth}, { 1,  0,  0} },
				{{ width, -height, -depth}, { 0, -1,  0} },
				{{ width, -height, -depth}, { 0,  0, -1} }
			};
			std::vector<uint32_t> indices
			{
				//up?
				1,4,7,10,1,7,
				//1,10,7,4,1,7,
				//front
				14, 2, 11,23,14,11,
				//right—¹
				21,9,6,18,21,6,
				//back—¹
				17,20,8,5,17,8,
				//left—¹
				12, 15, 3, 0, 12, 3,
				//bottom
				13,22,19,16,13,19
			};
			return std::make_unique<MeshData>(vertices, indices);
		}

		std::unique_ptr<MeshData> CreateSphere()
		{
			size_t stacks = 36;
			size_t slices = 18;
			float radius = 0.5f;
			const size_t vertexNum =  stacks * ( slices - static_cast<size_t>(1) ) + static_cast<size_t>(2);
			const size_t indexNum = static_cast<size_t>(6) * stacks * slices - static_cast<size_t>(1);
			std::vector<Argent::Data::ArMeshData::Vertex> vertices( vertexNum );
			std::vector<uint32_t> indices( indexNum );
			{


			const float theta = DirectX::XM_2PI / static_cast<float>(stacks);
			const float phi = DirectX::XM_PI / static_cast<float>(slices);
			int k = 0;

			vertices[k].position = { 0, radius, 0 };
			vertices[k].normal = { 0, 1, 0 };
			++k;
			for(int i = 0; i < slices - 1; ++i)
			{
				float y = radius * cosf( phi * static_cast<float>(i + 1) );
				float r = radius * sinf( phi * static_cast<float>(i + 1) );
				for(int j = 0; j < stacks; ++j)
				{
					vertices[k].position.x = { r * cosf(theta * j ) };
					vertices[k].position.y = { y };
					vertices[k].position.z = { -r * sinf( theta * j ) };

					DirectX::XMVECTOR VEC = XMLoadFloat3(&vertices[k].position);
					VEC = DirectX::XMVector3Normalize(VEC);
					XMStoreFloat3(&vertices[k].normal, VEC);

					++k;
				}
			}
			vertices[k].position = { 0, -radius, 0 };
			vertices[k].normal = { 0, -1, 0 };

			int offset = 1;
			int indNum = 0;
			for(int i = 0; i < 2 * (slices - 1); ++i)
			{
				for(int j = 0; j < stacks; ++j)
				{
					if( i == 0 )
					{
						indices[indNum] = 0;
						++indNum;

						indices[indNum] = static_cast<uint32_t>(j + 1);
						++indNum;

						if( j == stacks - 1)	indices[indNum] = 1;
						else						indices[indNum] = static_cast<uint32_t>(j + 2);
						++indNum;
					}
					else if( i == 2 * (slices - 1) - 1)
					{
						indices[indNum] = static_cast<uint32_t>(vertexNum -1);
						++indNum;

						indices[indNum] = static_cast<uint32_t>(vertexNum - 1 - offset);
						++indNum;

						if( j == stacks - 1)	indices[indNum] = static_cast<uint32_t>(vertexNum - 2);
						else					indices[indNum] = static_cast<uint32_t>(vertexNum - 1 - offset - 1);
						++indNum;

						offset++;
					}
					else
					{
						if( i % 2 == 1)
						{
							//upside
							if( j == stacks - 1)	indices[indNum] = static_cast<uint32_t>((i - 1) / 2 * stacks + 1);
							else					indices[indNum] = static_cast<uint32_t>((i - 1) / 2 * stacks + j + 2);
							++indNum;

							indices[indNum] = static_cast<uint32_t>(j + 1 + ( i / 2 ) * stacks);
							++indNum;

							indices[indNum] = static_cast<uint32_t>(j + 1 + ( i / 2 ) * stacks + stacks);
							++indNum;
						}
						else
						{
							indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j);
							++indNum;

							if(j == stacks - 1)		indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j - (stacks - 1));
							else					indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j + 1);
							++indNum;

							if(j == stacks - 1)		indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j - (stacks - 1) - stacks);
							else					indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j + 1 - stacks);
							++indNum;
						}
					}
				}
			}
			}
			return std::make_unique<MeshData>(vertices, indices);
		}

		std::unique_ptr<MeshData> CreateCapsule()
		{
			const size_t stacks = 36;
			const size_t slices = 18;
			const float radius = 0.5f;
			const float height = 2.0f;
			const size_t vertexCount =  stacks * ( slices - static_cast<size_t>(1) ) + static_cast<size_t>(2);
			const size_t indexCount = static_cast<size_t>(6) * stacks * slices - static_cast<size_t>(1);
			std::vector<Argent::Data::ArMeshData::Vertex> vertices(vertexCount);
			std::vector<uint32_t> indices(indexCount);
			const float theta = DirectX::XM_2PI / static_cast<float>(stacks);
			const float phi = DirectX::XM_PI / static_cast<float>(slices);
			int k = 0;

			vertices[k].position = { 0, height / 2, 0 };
			vertices[k].normal = { 0, 1, 0 };
			++k;
			for(int i = 0; i < slices - 1; ++i)
			{
				float y = radius * cosf( phi * static_cast<float>(i + 1) );

				y += y > 0 ? height / 2 - radius : -(height / 2 - radius);


				float r = radius * sinf( phi * static_cast<float>(i + 1) );
				for(int j = 0; j < stacks; ++j)
				{
					vertices[k].position.x = { r * cosf(theta * j ) };
					vertices[k].position.y = { y };
					vertices[k].position.z = { -r * sinf( theta * j ) };

					DirectX::XMVECTOR VEC = XMLoadFloat3(&vertices[k].position);
					VEC = DirectX::XMVector3Normalize(VEC);
					XMStoreFloat3(&vertices[k].normal, VEC);

					++k;
				}
			}
			vertices[k].position = { 0, -height / 2, 0 };
			vertices[k].normal = { 0, -1, 0 };

			int offset = 1;
			int indNum = 0;
			for(int i = 0; i < 2 * (slices - 1); ++i)
			{
				for(int j = 0; j < stacks; ++j)
				{
					if( i == 0 )
					{
						indices[indNum] = 0;
						++indNum;

						indices[indNum] = static_cast<uint32_t>(j + 1);
						++indNum;

						if( j == stacks - 1)	indices[indNum] = 1;
						else						indices[indNum] = static_cast<uint32_t>(j + 2);
						++indNum;
					}
					else if( i == 2 * (slices - 1) - 1)
					{
						indices[indNum] = static_cast<uint32_t>(vertexCount -1);
						++indNum;

						indices[indNum] = static_cast<uint32_t>(vertexCount - 1 - offset);
						++indNum;

						if( j == stacks - 1)	indices[indNum] = static_cast<uint32_t>(vertexCount - 2);
						else					indices[indNum] = static_cast<uint32_t>(vertexCount - 1 - offset - 1);
						++indNum;

						offset++;
					}
					else
					{
						if( i % 2 == 1)
						{
							//upside
							if( j == stacks - 1)	indices[indNum] = static_cast<uint32_t>((i - 1) / 2 * stacks + 1);
							else					indices[indNum] = static_cast<uint32_t>((i - 1) / 2 * stacks + j + 2);
							++indNum;

							indices[indNum] = static_cast<uint32_t>(j + 1 + ( i / 2 ) * stacks);
							++indNum;

							indices[indNum] = static_cast<uint32_t>(j + 1 + ( i / 2 ) * stacks + stacks);
							++indNum;
						}
						else
						{
							indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j);
							++indNum;

							if(j == stacks - 1)		indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j - (stacks - 1));
							else					indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j + 1);
							++indNum;

							if(j == stacks - 1)		indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j - (stacks - 1) - stacks);
							else					indices[indNum] = static_cast<uint32_t>(i / 2 * stacks + 1 + j + 1 - stacks);
							++indNum;
						}
					}
				}
			}
			return std::make_unique<MeshData>(vertices, indices);
		}
	}


	void ArResourceManager::Initialize()
	{
		Clear();
	
		meshData["Cube"] = Primitive::CreateCube();
		meshData["Sphere"] = Primitive::CreateSphere();
		meshData["Capsule"] = Primitive::CreateCapsule();
	}

	void ArResourceManager::Clear()
	{
		meshData.clear();
	}

	void ArResourceManager::Begin()
	{
	}


	MeshData* ArResourceManager::GetMeshData(const std::string& meshName) const
	{
		for(const auto& data : meshData)
		{
			if(data.first == meshName)
			{
				return data.second.get();
			}
		}
		return nullptr;
	}

	const MeshData* ArResourceManager::AddMeshData(const std::string& meshName,
		std::unique_ptr<MeshData> data)
	{
		return nullptr;
	}

	bool ArResourceManager::FindTexture(const wchar_t* filepath, ID3D12Resource** ppResource)
	{
		const std::wstring tmp = filepath;
		const auto it = textures.find(tmp);

		if (it != textures.end())
		{
			*ppResource = (*it).second.Get();
			(*it).second->AddRef();
			return true;
		}

		*ppResource = nullptr;
		return false;
	}
	
}
