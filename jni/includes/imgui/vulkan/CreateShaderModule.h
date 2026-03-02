

#ifndef TUTORIAL06_TEXTURE_CREATESHADERMODULE_H
#define TUTORIAL06_TEXTURE_CREATESHADERMODULE_H

#include <vulkan_wrapper.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>


VkResult buildShaderFromFile(
    android_app* appInfo,
    const char* filePath,
    VkShaderStageFlagBits type,
    VkDevice vkDevice,
    VkShaderModule* shaderOut);

#endif 
