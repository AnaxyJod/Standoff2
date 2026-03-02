#pragma once
#include "memory/Memory.h"
#include "memory/ProcessHelper.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Quaternion.h"
#include "imgui/draw/draw.h"
#include "memory/helper.h"
#include "../core/offsets.h"
#include "../core/config.h"
#include "../core/game_data.h"
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <type_traits>
#include <chrono>
#include <thread>
#include <deque>
#include <iomanip>

struct libraryInfo{
    uintptr_t start, end;

    libraryInfo() : start(0), end(0) {}
    libraryInfo(uintptr_t Start, uintptr_t End) : start(Start), end(End) {}
};


inline MemoryHelper mem;
inline libraryInfo libUnity;
inline int pid = -1;


class Il2cppHelper;
inline Il2cppHelper* helperPtr = nullptr;


namespace structs {
    template<typename TKey, typename TValue>
    struct [[maybe_unused]] Entry {
        int hashCode, next;
        TKey key;
        TValue value;
    };

    template<typename T>
    struct [[maybe_unused]] HashsetEntry {
        int hashCode, next;
        T value;
    };

    template<typename T>
    struct [[maybe_unused]] nullable {
        alignas(4) bool hasValue;
        T value;
    };

    struct [[maybe_unused]] safe {
        int salt, value;

        template<typename T>
        T get() {
            int result;
            if ((salt & 1) == std::is_same<T, float>::value)
                result = value ^ salt;
            else
                result = (*((uint8_t *) &(value) + 2)) | value && 0xFF00FF00 | ((*((uint8_t *) &value)) << 16);
            return *(T *) &result;
        }

        template<typename T>
        void set(T val) {
            salt = std::is_same<T, float>::value;
            value = *(int *) &val;
        }
    };

    struct [[maybe_unused]] object {
        uintptr_t instance;

        inline bool isValid() const;
        inline void getField(void *destination, uintptr_t offset, size_t size) const;

        template<typename T>
        T getField(uintptr_t offset) {
            T result{};
            getField(&result, offset, sizeof(T));
            return result;
        }
    };

    struct [[maybe_unused]] mstring : object {
        inline static void ConvertToUTF8(char *buf, const char16_t *str);
        inline static void ConvertToUTF16(char16_t *buf, const unsigned char *str);

        const char *c_str(size_t size = 256) const;
        std::string str(size_t size = 256) const;
    };

    struct [[maybe_unused]] marray : object {
        inline static uintptr_t cArrayOffset = 32;
        inline static uintptr_t countOffset = 0x18;
        inline static size_t mallocSize = 1024;

        inline int getCount();

        template<typename T>
        T get(int position) {
            if (isValid())
                return getField<T>(cArrayOffset + position * sizeof(T));
            return {};
        }

        template<typename T>
        inline void forEach(const std::function<bool(int, T)> &callback, bool useStaticBuffer = true, int *count = nullptr) {
            if (!isValid())
                return;
            auto i = getCount();
            if (count)
                *count = i;
            if (i > 0) {
                if (useStaticBuffer) {
                    static auto items_last = (T *) malloc(mallocSize);
                    auto size = fmin(mallocSize, i * sizeof(T));
                    getField(items_last, cArrayOffset, size);
                    for (int p = 0; p * sizeof(T) < size; p++)
                        if (!callback(p, items_last[p]))
                            break;
                } else
                    for (i--; i >= 0; i--)
                        if (!callback(i, get<T>(i)))
                            break;
            }
        }

        template<typename T>
        T find(const std::function<bool(int, T)> &callback, bool useStaticBuffer = true) {
            T result{};
            if (isValid())
                forEach<T>([&callback, &result](int position, T entry) {
                    if (entry.hashCode != 0 && callback(position, entry)) {
                        result = entry;
                        return false;
                    }
                    return true;
                }, useStaticBuffer);
            return result;
        }
    };

    struct [[maybe_unused]] mlist : object {
        inline static uintptr_t marrayOffset = 0x10;

        inline int getCount();

        template<typename T>
        T get(int position) {
            if (isValid())
                return getField<marray>(marrayOffset).get<T>(position);
            return {};
        }

        template<typename T>
        inline void forEach(const std::function<bool(int, T)> &callback, bool useStaticBuffer = true, int *count = nullptr) {
            if (isValid())
                getField<marray>(marrayOffset).forEach<T>(callback, useStaticBuffer, count);
        }

        template<typename T>
        T find(const std::function<bool(int, T)> &callback, bool useStaticBuffer = true) {
            T result{};
            if (isValid())
                getField<marray>(marrayOffset).forEach<T>([&callback, &result](int position, T entry) {
                    if (entry.hashCode != 0 && callback(position, entry)) {
                        result = entry;
                        return false;
                    }
                    return true;
                }, useStaticBuffer);
            return result;
        }
    };

    struct [[maybe_unused]] mdictionary : object {
        inline static uintptr_t marrayOffset = 0x18;

        inline int getCount();

        template<typename TKey, typename TValue>
        Entry<TKey, TValue> get(int position) {
            if (isValid())
                return getField<marray>(marrayOffset).get<Entry<TKey, TValue>>(position);
            return {};
        }

        template<typename TKey, typename TValue>
        inline void forEach(const std::function<bool(int, TKey, TValue)> &callback, bool useStaticBuffer = true, int *count = nullptr) {
            if (isValid())
                getField<marray>(marrayOffset).forEach<Entry<TKey, TValue>>([&callback](int position, Entry<TKey, TValue> entry) {
                    return entry.hashCode != 0 ? callback(position, entry.key, entry.value) : true;
                }, useStaticBuffer, count);
        }

        template<typename TKey, typename TValue>
        Entry<TKey, TValue> find(const std::function<bool(int, TKey, TValue)> &callback, bool useStaticBuffer = true) {
            Entry<TKey, TValue> result{};
            if (isValid())
                getField<marray>(marrayOffset).forEach<Entry<TKey, TValue>>([&callback, &result](int position, Entry<TKey, TValue> entry) {
                    if (entry.hashCode != 0 && callback(position, entry.key, entry.value)) {
                        result = entry;
                        return false;
                    }
                    return true;
                }, useStaticBuffer);
            return result;
        }
    };

    struct [[maybe_unused]] Vector4 {
        float x, y, z, w;

        Vector4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
        Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    };
    inline Vector4 operator*(Vector4 lhs, const float rhs) { return Vector4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }
    inline Vector4 operator+(Vector4 lhs, const float rhs) { return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs,lhs.w + rhs); }

    #include "math/Vector2.h"
    #include "math/Vector3.h"
    #include "math/Quaternion.h"

    struct [[maybe_unused]] Matrix {
        float _11, _12, _13, _14;
        float _21, _22, _23, _24;
        float _31, _32, _33, _34;
        float _41, _42, _43, _44;
    };

    struct [[maybe_unused]] TMatrix {
        Vector4 position;
        Quaternion rotation;
        Vector4 scale;
    };

    struct [[maybe_unused]] bounds {
        Vector3 center;
        Vector3 m_ext;
    };
}

inline bool Read(void *destination, uintptr_t source, size_t size) {
    struct iovec src[1], dst[1];
    src[0].iov_base = destination;
    src[0].iov_len = size;
    dst[0].iov_base = (void *)source;
    dst[0].iov_len = size;
    ssize_t bRead = syscall(SYS_process_vm_readv, pid, src, 1, dst, 1, 0);
    if (bRead == -1) {
        return false;
    }
    return bRead == sizeof(size);
}


namespace structs {
    inline bool object::isValid() const {
        return instance > 0;
    }

    inline void object::getField(void *destination, uintptr_t offset, size_t size) const {
        Read(destination, instance + offset, size);
    }

    inline void mstring::ConvertToUTF8(char *buf, const char16_t *str) {
        while (*str) {
            unsigned int codepoint = 0;

            if (*str <= 0xD7FF) {
                codepoint = *str;
                str++;
            } else if (*str <= 0xDBFF) {
                unsigned short highSurrogate = (*str - 0xD800) * 0x400, lowSurrogate = *(str + 1) - 0xDC00;
                codepoint = (lowSurrogate | highSurrogate) + 0x10000;
                str += 2;
            } else
                break;

            if (codepoint <= 0x007F && codepoint != 0)
                *buf++ = codepoint;
            else if (codepoint <= 0x07FF) {
                *buf++ = ((codepoint >> 6) & 0x1F) | 0xC0;
                *buf++ = ((codepoint) & 0x3F) | 0x80;
            } else if (codepoint <= 0xFFFF) {
                *buf++ = ((codepoint >> 12) & 0x0F) | 0xE0;
                *buf++ = ((codepoint >> 6) & 0x3F) | 0x80;
                *buf++ = ((codepoint) & 0x3F) | 0x80;
            } else if (codepoint <= 0x10FFFF) {
                *buf++ = ((codepoint >> 18) & 0x07) | 0xF0;
                *buf++ = ((codepoint >> 12) & 0x3F) | 0x80;
                *buf++ = ((codepoint >> 6) & 0x3F) | 0x80;
                *buf++ = ((codepoint) & 0x3F) | 0x80;
            } else
                break;
        }
        *buf = 0;
    }

    inline void mstring::ConvertToUTF16(char16_t *buf, const unsigned char *str) {
        while (*str) {
            unsigned char ch = *str;
            if (ch < 0x80) {
                *buf++ = (char16_t)ch;
                str++;
            } else
                break;
        }
        *buf = 0;
    }

    inline const char *mstring::c_str(size_t size) const {
        
        
        static thread_local char output[512];
        output[0] = 0;
        
        if (!isValid()) {
            return output;
        }

        auto input = new char16_t[size];
        Read(input, instance + 0x14, size);
        ConvertToUTF8(output, input);
        delete[] input;
        return output;
    }

    inline std::string mstring::str(size_t size) const {
        if (!isValid()) {
            return "";
        }
        
        std::string result;
        result.resize(size / 2 + 1);
        
        auto input = new char16_t[size];
        Read(input, instance + 0x14, size);
        
        char* output = new char[size / 2 + 1];
        ConvertToUTF8(output, input);
        result = output;
        
        delete[] input;
        delete[] output;
        return result;
    }

    inline int marray::getCount() {
        return getField<int>(marray::countOffset);
    }

    inline int mlist::getCount() {
        return getField<marray>(mlist::marrayOffset).getCount();
    }

    inline int mdictionary::getCount() {
        return getField<marray>(mdictionary::marrayOffset).getCount();
    }
}

struct Matrix {
    float m11, m12, m13, m14;
    float m21, m22, m23, m24;
    float m31, m32, m33, m34;
    float m41, m42, m43, m44;
};

struct TMatrix {
    Vector4 position;
    Quaternion rotation;
    Vector4 scale;
};

namespace DisplayCache {
    inline auto& GetDisplayInfo() {
        static draw::DisplayInfo displayInfo = draw::getDisplayInfo();
        if (displayInfo.width <= 0 || displayInfo.height <= 0) {
            displayInfo = draw::getDisplayInfo();
        }
        return displayInfo;
    }
    
    inline float GetScreenWidth() {
        return static_cast<float>(GetDisplayInfo().width);
    }
    
    inline float GetScreenHeight() {
        return static_cast<float>(GetDisplayInfo().height);
    }
}

inline ImVec2 worldToScreen(Vector3 pos, Matrix m, bool* c) {
    float screenWidth = DisplayCache::GetScreenWidth();
    float screenHeight = DisplayCache::GetScreenHeight();

    float screenX = (m.m11 * pos.x) + (m.m21 * pos.y) + (m.m31 * pos.z) + m.m41;
    float screenY = (m.m12 * pos.x) + (m.m22 * pos.y) + (m.m32 * pos.z) + m.m42;
    float screenW = (m.m14 * pos.x) + (m.m24 * pos.y) + (m.m34 * pos.z) + m.m44;
 
    float camX = screenWidth / 2;
    float camY = screenHeight / 2;
 
    float x = camX + (camX * screenX / screenW);
    float y = camY - (camY * screenY / screenW);
    *c = screenW > 0.0001f;
    return ImVec2(x,y);
}

inline Vector3 worldToScreen3(const Matrix &viewMatrix, const Vector3 &pos) {
    const float screenWidth = DisplayCache::GetScreenWidth();
    const float screenHeight = DisplayCache::GetScreenHeight();
    if (screenWidth <= 0 || screenHeight <= 0) {
        return {0.0f, 0.0f, -1.0f};
    }

    float screenW = viewMatrix.m14 * pos.x + viewMatrix.m24 * pos.y + viewMatrix.m34 * pos.z + viewMatrix.m44;
    if (screenW <= 0.0001f) {
        return {0.0f, 0.0f, -1.0f};
    }

    float screenX = viewMatrix.m11 * pos.x + viewMatrix.m21 * pos.y + viewMatrix.m31 * pos.z + viewMatrix.m41;
    float screenY = viewMatrix.m12 * pos.x + viewMatrix.m22 * pos.y + viewMatrix.m32 * pos.z + viewMatrix.m42;

    float x = screenWidth / 2.0f + (screenWidth / 2.0f) * screenX / screenW;
    float y = screenHeight / 2.0f - (screenHeight / 2.0f) * screenY / screenW;

    if (x < 0 || x > screenWidth || y < 0 || y > screenHeight) {
        return {0.0f, 0.0f, -1.0f};
    }

    return {x, y, screenW};
}

inline std::string convertUtf16ToUtf8(const std::u16string &utf16Str) {    
    if (utf16Str.empty()) {
        return std::string();
    }

    const char16_t *ptr = utf16Str.data();   
    std::u16string::size_type length = utf16Str.length();  
    if (ptr[0] == 0xFEFF) {      
        ptr += 1;      
        length -= 1;   
    } 

    std::string utf8Str;   
    utf8Str.reserve(length * 3);    
    char16_t currentChar;  
    for (std::u16string::size_type i = 0; i < length; ++i) {       
        currentChar = ptr[i];        
        if (currentChar < 0x0080) {          
            utf8Str.push_back((char)(currentChar & 0x00FF));          
            continue;       
        }      
        if (currentChar >= 0x0080 && currentChar <= 0x07FF) {          
            utf8Str.push_back((char)(((currentChar >> 6) & 0x1F) | 0xC0));        
            utf8Str.push_back((char)((currentChar & 0x3F) | 0x80));          
            continue;       
        }      
        if (currentChar >= 0xD800 && currentChar <= 0xDBFF) {           
            uint32_t highSurrogate = currentChar;          
            uint32_t lowSurrogate = ptr[++i];          
            uint32_t codePoint = highSurrogate - 0xD800;    
            codePoint <<= 10;        
            codePoint |= lowSurrogate - 0xDC00;      
            codePoint += 0x10000;           
            utf8Str.push_back((char)((codePoint >> 18) | 0xF0));     
            utf8Str.push_back((char)(((codePoint >> 12) & 0x3F) | 0x80));         
            utf8Str.push_back((char)(((codePoint >> 06) & 0x3F) | 0x80));            
            utf8Str.push_back((char)((codePoint & 0x3F) | 0x80));        
            continue;       
        }
        utf8Str.push_back((char)(((currentChar >> 12) & 0x0F) | 0xE0));          
        utf8Str.push_back((char)(((currentChar >> 6) & 0x3F) | 0x80));      
        utf8Str.push_back((char)((currentChar & 0x3F) | 0x80));         
        continue;
    }   
    return utf8Str;
}


struct GameString {
    uint64_t classPointer;
    uint64_t monitorData;
    int length;
    char firstChar[256];

    std::string getString() {
        
        if (length <= 0 || length > 127) {
            return "";
        }
        
        
        bool hasData = false;
        for (int i = 0; i < std::min(length * 2, 10); i++) {
            if (firstChar[i] != 0) {
                hasData = true;
                break;
            }
        }
        if (!hasData) return "";
        
        
        std::string result;
        result.reserve(length * 3);
        
        const char16_t* src = (const char16_t*)firstChar;
        for (int i = 0; i < length; i++) {
            char16_t ch = src[i];
            
            if (ch == 0) break; 
            
            if (ch < 0x80) {
                result.push_back((char)ch);
            } else if (ch < 0x800) {
                result.push_back((char)(0xC0 | (ch >> 6)));
                result.push_back((char)(0x80 | (ch & 0x3F)));
            } else if (ch >= 0xD800 && ch <= 0xDBFF && i + 1 < length) {
                
                char16_t low = src[i + 1];
                if (low >= 0xDC00 && low <= 0xDFFF) {
                    uint32_t cp = 0x10000 + ((ch - 0xD800) << 10) + (low - 0xDC00);
                    result.push_back((char)(0xF0 | (cp >> 18)));
                    result.push_back((char)(0x80 | ((cp >> 12) & 0x3F)));
                    result.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
                    result.push_back((char)(0x80 | (cp & 0x3F)));
                    i++; 
                }
            } else {
                result.push_back((char)(0xE0 | (ch >> 12)));
                result.push_back((char)(0x80 | ((ch >> 6) & 0x3F)));
                result.push_back((char)(0x80 | (ch & 0x3F)));
            }
        }
        
        return result;
    }
    
    inline static uint64_t getPhotonPointer(uint64_t player) { 
        if (player == 0 || player < 0x10000 || player > 0x7fffffffffff) return 0;
        uint64_t photon = mem.read<uint64_t>(player + offsets::photonPointer);
        if (photon < 0x10000 || photon > 0x7fffffffffff) return 0;
        return photon;
    }

    template<typename T>
    static T getPlayerProperty(uint64_t player, const char* propertyName) { 
        T propertyValue = T();
        if (player == 0 || player < 0x10000 || player > 0x7fffffffffff) return propertyValue;
        
        uint64_t photon = getPhotonPointer(player);
        if (photon == 0) return propertyValue;
        
        uint64_t properties = mem.read<uint64_t>(photon + 0x38);
        if (properties == 0 || properties < 0x10000 || properties > 0x7fffffffffff) return propertyValue;
        
        int propertyCount = mem.read<int>(properties + 0x20);
        if (propertyCount <= 0 || propertyCount > 100) return propertyValue;
        
        uint64_t entriesArray = mem.read<uint64_t>(properties + 0x18);
        if (entriesArray == 0 || entriesArray < 0x10000 || entriesArray > 0x7fffffffffff) return propertyValue;
        
        for (int i = 0; i < propertyCount; i++) {
            uint64_t propertyKey = mem.read<uint64_t>(entriesArray + 0x28 + 0x18 * i); 
            
            
            if (propertyKey == 0 || propertyKey < 0x10000 || propertyKey > 0x7fffffffffff) continue;
            
            std::string keyValue = mem.read<GameString>(propertyKey).getString(); 
            if (!keyValue.empty() && strstr(keyValue.c_str(), propertyName)) {
                uint64_t propertyValuePtr = mem.read<uint64_t>(entriesArray + 0x30 + 0x18 * i);
                if (propertyValuePtr != 0 && propertyValuePtr >= 0x10000 && propertyValuePtr <= 0x7fffffffffff) {
                    propertyValue = mem.read<T>(propertyValuePtr + 0x10);
                }
                break; 
            }
        }
        return propertyValue; 
    }

    template<typename T>
    inline static void set_prop(uint64_t player, const char* tag, T value) {
        if (player == 0 || player < 0x10000 || player > 0x7fffffffffff) return;
        
        uint64_t photon = getPhotonPointer(player);
        if (photon == 0) return;
        
        uint64_t props = mem.read<uint64_t>(photon + 0x38);
        if (props == 0 || props < 0x10000 || props > 0x7fffffffffff) return;
        
        int size = mem.read<int>(props + 0x20);
        if (size <= 0 || size > 100) return;
        
        uint64_t entriesArray = mem.read<uint64_t>(props + 0x18);
        if (entriesArray == 0 || entriesArray < 0x10000 || entriesArray > 0x7fffffffffff) return;
        
        for (int i = 0; i < size; i++) {
            uint64_t propkey = mem.read<uint64_t>(entriesArray + 0x28 + 0x18 * i);
            if (propkey == 0 || propkey < 0x10000 || propkey > 0x7fffffffffff) continue;
            
            std::string keyVal = mem.read<GameString>(propkey).getString();
            if (!keyVal.empty() && strstr(keyVal.c_str(), tag)) {
                uint64_t propval = mem.read<uint64_t>(entriesArray + 0x30 + 0x18 * i);
                if (propval != 0 && propval >= 0x10000 && propval <= 0x7fffffffffff) {
                    mem.write<T>(propval + 0x10, value);
                }
                break;
            }
        }
    }
    
    inline static Vector3 GetPosition(uint64_t transObj2) {
        if (transObj2 == 0 || transObj2 < 0x10000 || transObj2 > 0x7fffffffffff) return Vector3();
        
        uint64_t transObj = mem.read<uint64_t>(transObj2 + 0x10);
        if (transObj == 0 || transObj < 0x10000 || transObj > 0x7fffffffffff) return Vector3();
        
        uint64_t matrix = mem.read<uint64_t>(transObj + 0x38);
        if (matrix == 0 || matrix < 0x10000 || matrix > 0x7fffffffffff) return Vector3();
        
        uint64_t index = mem.read<uint64_t>(transObj + 0x40);
        uint64_t matrix_list = mem.read<uint64_t>(matrix + 0x18);
        uint64_t matrix_indices = mem.read<uint64_t>(matrix + 0x20);
        
        if (matrix_list == 0 || matrix_list < 0x10000 || matrix_list > 0x7fffffffffff) return Vector3();
        if (matrix_indices == 0 || matrix_indices < 0x10000 || matrix_indices > 0x7fffffffffff) return Vector3();
        
        
        if (index > 10000) return Vector3();
        
        Vector3 result = mem.read<Vector3>(matrix_list + sizeof(TMatrix) * index);
        int transformIndex = mem.read<int>(matrix_indices + sizeof(int) * index);
        
        int maxIterations = 50; 
        while(transformIndex >= 0 && transformIndex < 10000 && maxIterations-- > 0) {
            TMatrix tMatrix = mem.read<TMatrix>(matrix_list + sizeof(TMatrix) * transformIndex);
     
            float rotX = tMatrix.rotation.x;
            float rotY = tMatrix.rotation.y;
            float rotZ = tMatrix.rotation.z;
            float rotW = tMatrix.rotation.w;
     
            float scaleX = result.x * tMatrix.scale.x;
            float scaleY = result.y * tMatrix.scale.y;
            float scaleZ = result.z * tMatrix.scale.z;
     
            result.x = tMatrix.position.x + scaleX + (scaleX * ((rotY * rotY * -2.0) - (rotZ * rotZ * 2.0))) + (scaleY * ((rotW * rotZ * -2.0) - (rotY * rotX * -2.0))) + (scaleZ * ((rotZ * rotX * 2.0) - (rotW * rotY * -2.0)));
            result.y = tMatrix.position.y + scaleY + (scaleX * ((rotX * rotY * 2.0) - (rotW * rotZ * -2.0))) + (scaleY * ((rotZ * rotZ * -2.0) - (rotX * rotX * 2.0))) + (scaleZ * ((rotW * rotX * -2.0) - (rotZ * rotY * -2.0)));
            result.z = tMatrix.position.z + scaleZ + (scaleX * ((rotW * rotY * -2.0) - (rotX * rotZ * -2.0))) + (scaleY * ((rotY * rotZ * 2.0) - (rotW * rotX * -2.0))) + (scaleZ * ((rotX * rotX * -2.0) - (rotY * rotY * 2.0)));
     
            transformIndex = mem.read<int>(matrix_indices + sizeof(int) * transformIndex);
        }
        return result;
    }
};

inline int CalcValue(int a1, int a2) { 
    if ((a1 & 1) != 0) { 
        return a1 ^ a2; 
    } else { 
        return (a2 & 0xFF00FF00) | ((uint8_t)a2 << 16) | ((a2 >> 16) & 0xFF); 
    } 
}

inline int ReadSafe(uintptr_t address) {
    int a1 = mem.read<int>(address);
    int a2 = mem.read<int>(address + 0x4);
    return CalcValue(a1, a2);
}
  
inline void WriteSafe(uintptr_t address, int value) {
    int a1 = mem.read<int>(address);
    int encodedValue = CalcValue(a1, value);
    mem.write<int>(address + 0x4, encodedValue);
}

inline Vector3 FastWorldToScreen(const Vector3& pos, const Matrix& m) {
    float screenW = m.m14 * pos.x + m.m24 * pos.y + m.m34 * pos.z + m.m44;
    if (screenW <= 0.0001f) return Vector3(0, 0, -1);

    float screenX = m.m11 * pos.x + m.m21 * pos.y + m.m31 * pos.z + m.m41;
    float screenY = m.m12 * pos.x + m.m22 * pos.y + m.m32 * pos.z + m.m42;

    float screenWidth = DisplayCache::GetScreenWidth();
    float screenHeight = DisplayCache::GetScreenHeight();

    return Vector3(
        screenWidth / 2.0f + (screenWidth / 2.0f) * screenX / screenW,
        screenHeight / 2.0f - (screenHeight / 2.0f) * screenY / screenW,
        screenW
    );
}


class g_functions{
    public:
        class g_esp{
            public:
                void Update(Vector3 Position, int health, int team, int localTeam, int armor, int ping, int money, std::string name, std::string weapon, uint64_t player, uint64_t localPlayer);
                void DrawOutlinedText(ImFont* font, const char* text, ImVec2 pos, ImU32 color);
                void DrawText(ImFont* font, const char* text, ImVec2 pos, ImU32 color);
                void DrawOutlinedIcon(ImFont* font, const char* icon, ImVec2 pos, ImU32 color, float fontSize);
                
                void ApplyRainbowToAllColors(float speed);
                void RainbowCross(float speed);
                bool rainbow_enabled;
                bool rainbow_cross_enabled;
                
                bool enabled;
                bool box;
                int box_type = 0;  
                float corner_length = 0.3f;  
                bool box3d_rot;
                bool box3d_ray;
                bool box_filled;
                bool tracer;
                int tracer_start_pos = 0;  
                int tracer_end_pos = 0;    
                bool skeleton;
                bool footstep;
                bool footstep_exist;
                bool health_bar;
                bool health_text;
                bool armor_bar;
                bool armor_text;
                bool name;
                bool host;
                bool weapon;
                bool weapon_text;
                bool money;
                bool custom_cross;
                bool night_mode;
                
                float round = 0.0f;
                float box_width_ratio = 4.0f;  
                
                float footstep_delta = 0.25f;
                float footstep_speed = 1.2f;
                
                
                float box_width = 2.0f;
                float box_outline_width = 2.0f;
                bool box_outline_enabled = false;
                float skeleton_width = 2.0f;
                bool skeleton_outline_enabled = true;
                float tracer_width = 3.0f;
                float health_bar_width = 0.52f;  
                bool health_bar_outline_enabled = true;
                bool health_bar_gradient = false;
                float armor_bar_width = 0.52f;   
                bool armor_bar_outline_enabled = true;
                bool armor_bar_gradient = false;
                
                
                float health_bar_offset = 0.5f;  
                float health_bar_position_offset = 0.0f;  
                float armor_bar_offset = 0.5f;   
                float name_offset = 1.0f;        
                float weapon_name_offset = 1.0f; 
                float weapon_icon_offset = 1.0f; 
                float weapon_icon_size = 1.0f;   
                
                
                int health_bar_position = 0;     
                int armor_bar_position = 3;      
                
                
                int name_position = 0;           
                int weapon_name_position = 1;    
                
                
                bool offscreen = false;
                bool offscreen_force = false;    
                float offscreen_radius = 100.0f; 
                float offscreen_size = 20.0f;    
                
                
                bool visible_check = false;      
                
                
                bool bomber = false;
                float bomber_offset = 3.0f;      
                int bomber_position = 1;         
                
                
                bool defuser = false;
                float defuser_offset = 3.0f;     
                int defuser_position = 1;        

                
                float box_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float tracer_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float offscreen_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
                float bomber_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };  
                float defuser_color[4] = { 0.0f, 0.5f, 1.0f, 1.0f };  
                float weapon_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float weapon_color_text[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float name_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float skeleton_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float footstep_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float health_bar_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float armor_bar_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float filled_color[4] = { 1.0f, 1.0f, 1.0f, 0.5f };
                float distance_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float money_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float ping_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float dropped_weapon_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float cross_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                
                
                float box_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float tracer_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float offscreen_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float bomber_color_visible[4] = { 1.0f, 0.5f, 0.0f, 1.0f };
                float defuser_color_visible[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
                float weapon_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float weapon_color_text_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float name_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float skeleton_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float footstep_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float health_bar_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float armor_bar_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float filled_color_visible[4] = { 0.0f, 1.0f, 0.0f, 0.5f };
                float distance_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float money_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float ping_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float dropped_weapon_color_visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                
                
                bool rainbow_box = false;
                bool rainbow_filled = false;
                bool rainbow_footstep = false;
                bool rainbow_skeleton = false;
                bool rainbow_tracer = false;
                bool rainbow_offscreen = false;
                bool rainbow_health_bar = false;
                bool rainbow_armor_bar = false;
                bool rainbow_name = false;
                bool rainbow_distance = false;
                bool rainbow_money = false;
                bool rainbow_ping = false;
                bool rainbow_bomber = false;
                bool rainbow_defuser = false;
                bool rainbow_weapon = false;
                bool rainbow_weapon_text = false;
                bool rainbow_dropped_weapon = false;
                
                
                bool rainbow_box_visible = false;
                bool rainbow_filled_visible = false;
                bool rainbow_footstep_visible = false;
                bool rainbow_skeleton_visible = false;
                bool rainbow_tracer_visible = false;
                bool rainbow_offscreen_visible = false;
                bool rainbow_health_bar_visible = false;
                bool rainbow_armor_bar_visible = false;
                bool rainbow_name_visible = false;
                bool rainbow_distance_visible = false;
                bool rainbow_money_visible = false;
                bool rainbow_ping_visible = false;
                bool rainbow_bomber_visible = false;
                bool rainbow_defuser_visible = false;
                bool rainbow_weapon_visible = false;
                bool rainbow_weapon_text_visible = false;
                bool rainbow_dropped_weapon_visible = false;
                
                float rainbow_speed = 1.0f;

                class h_flags{
                    public:
                        bool money = false;
                        bool hk = false;
                        bool ping = false;
                        bool distance = false;
                        float offset = 3.0f;
                        int position = 1;  
                        
                        int order[3] = {-1, -1, -1};  
                        int nextOrder = 0;
                } flags;
            
                bool greandes_view;
                bool dropped_weapon;
        } esp;
        
        Matrix viewMatrix;
        int playerCount;
        bool show_menu = true;
        bool isAlive = false;
        bool infinityAmmo = false;
        bool moneyHack = false;
        bool wallshot = false;
        bool fireRate = false;
        bool noRecoil = false;
        int selectedKnifeSkin = 0; 
        float distance;
        float fontSize;
        

}; inline g_functions functions;

struct BoneBatchData {
    uint64_t transforms[13];
    Vector3 worldPositions[13];
    ImVec2 screenPositions[13];
    bool visibles[13];
};

BoneBatchData ReadAllBonesOptimized(uint64_t player);

struct EnemySpeedTracker {
    struct EnemyData {
        Vector3 lastPosition;
        std::chrono::steady_clock::time_point lastTime;
        float lastSpeed = 0.0f;
        bool wasMoving = false;
        bool firstUpdate = true;
    };
    
    std::unordered_map<uint64_t, EnemyData> enemyData;
    
    float GetEnemySpeed(uint64_t enemyPtr, const Vector3& currentPos) {
        auto currentTime = std::chrono::steady_clock::now();
        
        auto& data = enemyData[enemyPtr];
        
        if (data.firstUpdate) {
            data.lastPosition = currentPos;
            data.lastTime = currentTime;
            data.firstUpdate = false;
            return 0.0f;
        }
        
        auto timeDiff = std::chrono::duration_cast<std::chrono::microseconds>(
            currentTime - data.lastTime).count() / 1000.0f;
        
        float speed = 0.0f;
        
        if (timeDiff >= 0.1f) {
            Vector3 deltaPos = currentPos - data.lastPosition;
            float distance = deltaPos.length();
            
            float timeInSeconds = timeDiff / 1000.0f;
            speed = distance / timeInSeconds;
            
            if (speed >= 9.0f) {
                speed = data.lastSpeed;
            }
            
            data.lastPosition = currentPos;
            data.lastTime = currentTime;
            data.lastSpeed = speed;
            data.wasMoving = (speed > 1.0f);
            
            return speed;
        }
        
        return data.lastSpeed;
    }
    
    bool WasMoving(uint64_t enemyPtr) {
        auto it = enemyData.find(enemyPtr);
        if (it != enemyData.end()) {
            return it->second.wasMoving;
        }
        return false;
    }
    
    void RemoveEnemy(uint64_t enemyPtr) {
        enemyData.erase(enemyPtr);
    }
    
    void ResetEnemy(uint64_t enemyPtr) {
        auto it = enemyData.find(enemyPtr);
        if (it != enemyData.end()) {
            it->second.firstUpdate = true;
            it->second.lastSpeed = 0.0f;
        }
    }
};

inline EnemySpeedTracker enemySpeed;

struct PlayerHealthState {
    int lastHealth = 100;
    int lastArmor = 0;
    std::string name = "";
    bool isValid = false;
};

inline std::unordered_map<uint64_t, PlayerHealthState> playerHealthStates;

inline uint64_t getPhotonPlayer(uint64_t playerController) {
    if (!Config::IsValidPtr(playerController)) return 0;
    uint64_t photon = mem.read<uint64_t>(playerController + offsets::photonPointer);
    if (!Config::IsValidPtr(photon)) return 0;
    return photon;
}

inline uint64_t getLastAttacker(uint64_t playerController) {
    if (!Config::IsValidPtr(playerController)) return 0;
    uint64_t hitController = mem.read<uint64_t>(playerController + offsets::playerHitController);
    if (Config::IsValidPtr(hitController)) {
        uint64_t attacker = mem.read<uint64_t>(hitController + offsets::lastAttacker);
        if (Config::IsValidPtr(attacker)) {
            return attacker;
        }
    }
    return 0;
}

inline Vector3 getHitPoint(uint64_t victimPlayer) {
    Vector3 hitPoint = Vector3::Zero();
    if (!Config::IsValidPtr(victimPlayer)) return hitPoint;
    
    uint64_t victimHitController = mem.read<uint64_t>(victimPlayer + offsets::playerHitController);
    if (Config::IsValidPtr(victimHitController)) {
        uint64_t lastHitData = mem.read<uint64_t>(victimHitController + 0xE8);
        if (Config::IsValidPtr(lastHitData)) {
            uint64_t hitsArray = mem.read<uint64_t>(lastHitData + 0x38);
            if (Config::IsValidPtr(hitsArray)) {
                int hitsCount = mem.read<int>(hitsArray + 0x18);
                if (hitsCount > 0 && hitsCount < 100) {
                    uint64_t firstHit = mem.read<uint64_t>(hitsArray + 0x20);
                    if (Config::IsValidPtr(firstHit)) {
                        hitPoint = mem.read<Vector3>(firstHit + 0x1C);
                    }
                }
            }
            if (hitPoint == Vector3::Zero()) {
                hitPoint = mem.read<Vector3>(lastHitData + 0x10);
            }
        }
    }
    
    return hitPoint;
}

inline Vector3 getPlayerPosition(uint64_t player) {
    if (!Config::IsValidPtr(player)) return Vector3::Zero();
    
    uint64_t movementController = mem.read<uint64_t>(player + offsets::movementController);
    if (!Config::IsValidPtr(movementController)) return Vector3::Zero();
    
    uint64_t transformData = mem.read<uint64_t>(movementController + Offsets::Movement::transformData);
    if (!Config::IsValidPtr(transformData)) return Vector3::Zero();
    
    return mem.read<Vector3>(transformData + Offsets::Movement::position);
}


inline bool HasBombInInventory(uint64_t player) {
    if (!Config::IsValidPtr(player)) return false;
    
    uint64_t weaponryController = mem.read<uint64_t>(player + 0x88);
    if (!Config::IsValidPtr(weaponryController)) return false;
    
    uint64_t weaponDict = mem.read<uint64_t>(weaponryController + 0x58);
    if (!Config::IsValidPtr(weaponDict)) return false;
    
    uint64_t entries = mem.read<uint64_t>(weaponDict + 0x18);
    if (!Config::IsValidPtr(entries)) return false;
    
    int count = mem.read<int>(weaponDict + 0x20);
    if (count <= 0 || count > 20) return false;
    
    for (int i = 0; i < count; i++) {
        uint64_t weaponController = mem.read<uint64_t>(entries + 0x20 + i * 0x18 + 0x10);
        if (!Config::IsValidPtr(weaponController)) continue;
        
        uint64_t weaponParams = mem.read<uint64_t>(weaponController + 0xA8);
        if (!Config::IsValidPtr(weaponParams)) continue;
        
        uint8_t weaponId = mem.read<uint8_t>(weaponParams + 0x18);
        if (weaponId == 100) return true;
    }
    
    return false;
}

inline int32_t GetKitId(uint64_t player) {
    if (!Config::IsValidPtr(player)) return -1;
    
    uint64_t weaponryController = mem.read<uint64_t>(player + 0x88);
    if (!Config::IsValidPtr(weaponryController)) return -2;
    
    uint64_t kitController = mem.read<uint64_t>(weaponryController + 0x98);
    if (!Config::IsValidPtr(kitController)) return -3;
    
    uint64_t weaponParams = mem.read<uint64_t>(kitController + 0xA8);
    if (!Config::IsValidPtr(weaponParams)) return -5;
    
    return mem.read<int32_t>(weaponParams + 0x18);
}

inline bool HasDefuseKitInInventory(uint64_t player) {
    int32_t kitId = GetKitId(player);
    return (kitId == 102);
}


void InfinityAmmo();


void MoneyHack();


void Wallshot();


void FireRateHack();


void NoRecoil();





void ApplyKnifeSkin();


uint64_t GetPhotonPlayer(uint64_t player);
uint64_t GetRoom();
bool IsHost(uint64_t photonPlayer);
bool IsPlayerHost(uint64_t player);
void HostIndicator();

