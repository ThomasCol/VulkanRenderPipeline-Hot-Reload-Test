#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <unordered_map>

Mesh& Mesh::LoadMesh(const char* modelFile, const char* textureFile)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH))
		throw std::runtime_error(warn + err);

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex = {};


			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
				_vertices.push_back(vertex);
			}
			_indices.push_back(uniqueVertices[vertex]);
		}
	}

	_bindingDescriptor = Vertex::GetBindingDescription();
	_attributeDescriptions = Vertex::GetAttributeDescriptions();
	_info = {};
	_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	_info.vertexBindingDescriptionCount = 1;
	_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributeDescriptions.size());
	_info.pVertexBindingDescriptions = &_bindingDescriptor;
	_info.pVertexAttributeDescriptions = _attributeDescriptions.data();

	_texture.Load(textureFile);
}

void Mesh::CreateBuffers(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicQueue, VkCommandPool commandPool)
{
	CreateVertexBuffer(device, physicalDevice, graphicQueue, commandPool);
	CreateIndexBuffer(device, physicalDevice, graphicQueue, commandPool);
	_texture.CreateTexture(device, physicalDevice, commandPool, graphicQueue);
}

void Mesh::CreateVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicQueue, VkCommandPool commandPool)
{
	VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();

	Buffer stagingBuffer;
	stagingBuffer.CreateBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer.MapMemory(device, 0, bufferSize, 0, _vertices.data());

	_vertexBuffer.CreateBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	stagingBuffer.CopyBuffer(device, graphicQueue, commandPool, _vertexBuffer, bufferSize);

	stagingBuffer.Destroy(device);
}

void Mesh::CreateIndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicQueue, VkCommandPool commandPool)
{
	VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();

	Buffer stagingBuffer;
	stagingBuffer.CreateBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer.MapMemory(device, 0, bufferSize, 0, _indices.data());

	_indexBuffer.CreateBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	stagingBuffer.CopyBuffer(device, graphicQueue, commandPool, _indexBuffer, bufferSize);

	stagingBuffer.Destroy(device);
}

void Mesh::Destroy(VkDevice device)
{
	_indexBuffer.Destroy(device);
	_vertexBuffer.Destroy(device);
	_texture.Destroy(device);
}