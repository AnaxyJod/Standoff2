#include "helper.h"
#include "../uses.h"

std::string Il2cppHelper::readString(uintptr_t address) {
    if (!address) return "";
    
    uintptr_t str = mem.read<uintptr_t>(address);
    if (!str) return "";
    
    int32_t length = mem.read<int32_t>(str + 0x10);
    if (length <= 0 || length > 256) return "";
    
    char16_t buffer[256];
    mem.readBuffer(str + 0x14, buffer, length * 2);
    buffer[length] = 0;
    
    std::string result;
    for (int i = 0; i < length; i++) {
        if (buffer[i] < 128) {
            result += (char)buffer[i];
        }
    }
    
    return result;
}

uintptr_t Il2cppHelper::getInstance(uintptr_t typeInfoAddress, bool hasParent, uintptr_t offset) {
    uintptr_t target_class = mem.read<uintptr_t>(typeInfoAddress);
    if (!target_class) return 0;
    
    if (hasParent) {
        
        
        target_class = mem.read<uintptr_t>(sizeof(uintptr_t) + target_class + offsetof(Il2CppClass, parent));
        if (!target_class) return 0;
    }
    
    
    uintptr_t static_fields = mem.read<uintptr_t>(sizeof(uintptr_t) + target_class + offsetof(Il2CppClass, static_fields));
    if (!static_fields) return 0;
    
    uintptr_t instance = mem.read<uintptr_t>(static_fields + offset);
    if (!instance) return 0;
    
    return instance;
}
