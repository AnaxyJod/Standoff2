#pragma once
#include "il2cpp.h"
#include "Memory.h"
#include <vector>
#include <map>
#include <string>


extern MemoryHelper mem;

class Il2cppHelper {
public:
    Il2cppHelper() {}

    template<typename T>
    std::vector<T> readArray(uintptr_t address);

    template<typename T>
    std::vector<T> readList(uintptr_t address);

    template<typename K, typename V>
    std::map<K, V> readDictionary(uintptr_t address);

    std::string readString(uintptr_t address); 

    uintptr_t getInstance(uintptr_t typeInfoAddress, bool hasParent = true, uintptr_t offset = 0x0);
};

template<typename T>
std::vector<T> Il2cppHelper::readArray(uintptr_t address) {
    std::vector<T> result;
    if (!address) return result;

    uintptr_t array = mem.read<uintptr_t>(address);
    if (!array) return result;

    int32_t length = mem.read<int32_t>(array + 0x1C);

    if (length <= 0) return result;
    for (int i = 0; i < length; i++) {
        T value = mem.read<T>(array + 0x20 + sizeof(T) * i);
        result.push_back(value);
    }

    return result;
}

template<typename T>
std::vector<T> Il2cppHelper::readList(uintptr_t address) {
    std::vector<T> result;
    if (!address) return result;

    uintptr_t list = mem.read<uintptr_t>(address);
    if (!list) return result;

    int32_t count = mem.read<int32_t>(list + 0xC);
    if (count <= 0) return result;

    uintptr_t array = mem.read<uintptr_t>(list + 0x8);
    if (!array) return result;

    for (int i = 0; i < count; i++) {
        T value = mem.read<T>(array + 0x20 + sizeof(T) * i);
        result.push_back(value);
    }

    return result;
}

struct Entry {
    int32_t hashCode;
    int32_t next;
    int key;
    Il2CppObject* value;
};

template<typename K, typename V>
std::map<K, V> Il2cppHelper::readDictionary(uintptr_t address) {
    std::map<K, V> result;
    if (!address) return result;

    uintptr_t dict = mem.read<uintptr_t>(address);
    if (!dict) return result;

    int32_t count = mem.read<int32_t>(dict + 0x20);
    if (count <= 0) return result;

    uintptr_t entries = mem.read<uintptr_t>(dict + 0x18);
    if (!entries) return result;

    for (int i = 0; i < count; i++) {
        Entry entry = mem.read<Entry>(entries + 0x20 + sizeof(entry) * i);

        K key = (K) entry.key;
        V value = (V) entry.value;

        result.insert(std::pair<K, V>(key, value));
    }

    return result;
}


inline Il2cppHelper helper;
