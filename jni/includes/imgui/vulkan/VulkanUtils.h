#ifndef __VULKANUTILS_G_H__
#define __VULKANUTILS_G_H__


#include "imgui.h"
#include "vulkan_wrapper.h"


struct MyTextureData {
    VkDescriptorSet DS;         
    int Width;
    int Height;
    int Channels;

    
    VkImageView ImageView;
    VkImage Image;
    VkDeviceMemory ImageMemory;
    VkSampler Sampler;
    VkBuffer UploadBuffer;
    VkDeviceMemory UploadBufferMemory;

    MyTextureData() { memset(this, 0, sizeof(*this)); }
};

bool LoadTextureFromFile(const char *filename, MyTextureData *tex_data);

bool LoadTextureFromMemory(const void *filedata, int len, MyTextureData *tex_data);

void RemoveTexture(MyTextureData *tex_data);

void SetupVulkan();

void SetupVulkanWindow(ANativeWindow *window, int width, int height);

void UploadFonts();

void SwapChainRebuild(int w, int h);

void FrameRender(ImDrawData *draw_data);

void FramePresent();

void DeviceWait();

void CleanupVulkanWindow();

void CleanupVulkan();

#endif 
