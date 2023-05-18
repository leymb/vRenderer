#pragma once
#include "Buffer.h"
class ShaderStorageBuffer :
    public Buffer
{
public:
    ShaderStorageBuffer();
    ~ShaderStorageBuffer();

    /// <summary>
    /// 	Creates a SSBO with the data provided. Automatically creates a staging buffer and uploads
    /// 	the data.
    /// </summary>
    /// <param name="a_Data">		  	[in,out] If non-null, the data that should be uploaded to the
    /// 								GPU.</param>
    /// <param name="a_BufferSize">   	Size of the buffer.</param>
    /// <param name="a_Device">		  	The device.</param>
    /// <param name="a_GraphicsQueue">	Graphics Queue.</param>
    /// <param name="a_CommandPool">  	The command pool.</param>

    void Create(void* a_Data, VkDeviceSize a_BufferSize, const Device& a_Device, VkQueue a_GraphicsQueue, VkCommandPool a_CommandPool);

    /// <summary>
    /// 	Creates a SSBO with the data provided. The staging buffer passed in will not be consumed
    /// 	and can be reused for several SSBOs.
    /// </summary>
    /// <param name="a_StagingBuffer">	Buffer for staging the data that will be uploaded to the GPU.</param>
    /// <param name="a_BufferSize">   	Size of the buffer.</param>
    /// <param name="a_Device">		  	The device.</param>
    /// <param name="a_GraphicsQueue">	Graphics Queue.</param>
    /// <param name="a_CommandPool">  	The command pool.</param>

    void CreateWithStagingBuffer(const Buffer& a_StagingBuffer, VkDeviceSize a_BufferSize, const Device& a_Device, VkQueue a_GraphicsQueue,
                                 VkCommandPool a_CommandPool);

    VkDescriptorSetLayout CreateDescriptorSetLayout(const VkDevice a_Device);
};

