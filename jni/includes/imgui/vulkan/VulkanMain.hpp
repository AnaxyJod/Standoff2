












#ifndef __VULKANMAIN_HPP__
#define __VULKANMAIN_HPP__

#include <android/native_window.h>



bool InitVulkan(ANativeWindow *window);


void DeleteVulkan(void);


bool IsVulkanReady(void);


bool VulkanDrawFrame(void);

#endif 


