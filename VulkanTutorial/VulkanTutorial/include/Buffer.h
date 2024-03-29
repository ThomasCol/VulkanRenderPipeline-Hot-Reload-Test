#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Context.h"
#include "CommandPool.h"

class Buffer
{
public:
	Buffer() = default;
	~Buffer() = default;

	Buffer& CreateBuffer(Context context, VkDeviceSize size, VkBufferUsageFlags usage,
					VkMemoryPropertyFlags properties);
	uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void MapMemory(VkDevice device, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void* data);

	void CopyBuffer(Context context, Buffer& dstBuffer, VkDeviceSize size);

	void Destroy(VkDevice device);

	inline const VkBuffer& GetBuffer() { return _buffer; };

private:
	VkBuffer						_buffer;
	VkDeviceMemory					_bufferMemory;
};