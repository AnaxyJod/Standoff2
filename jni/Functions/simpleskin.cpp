#include "../includes/uses.h"
#include "../includes/players.h"
#include <cstdlib>
#include <ctime>

static inline bool isValidPtr(uint64_t ptr) {
    return ptr != 0 && ptr >= 0x10000 && ptr <= 0x7fffffffffff;
}


namespace SimpleSkinOffsets {
    constexpr int inventoryService_addr = 135055488;
    constexpr int inventorySkinsDict = 0xF8;
    constexpr int invItem_id = 0x10;
}


static const int skinIDs[] = {
    
    71001,  
    72002,  
    47502,  
    73001,  
    77813,  
    87919,  
    67701,  
    138000, 
    
    3000,   
    3001,   
    3002,   
    3003,   
    3004,   
    3005,   
    3006,   
    3007,   
    3008,   
    3009,   
    3010,   
    3011,   
    3012,   
    3013,   
    3014,   
    3015,   
    3016,   
    3017,   
    3018,   
    3019,   
    3020    
};


template <typename TKey, typename TValue>
class UnityDict {
public:
    static int Count(uint64_t dict_ptr) {
        if (!dict_ptr) return 0;
        int ret = mem.read<int>(dict_ptr + 0x20);
        return ret >= 1 ? ret : 0;
    }
    
    static TValue GetValue(uint64_t dict_ptr, size_t idx) {
        uint64_t entries = mem.read<uint64_t>(dict_ptr + 0x18);
        return mem.read<TValue>(entries + 0x20 + 0x10 + 0x18 * idx);
    }
};

void ApplyKnifeSkin() {
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
    
    uint64_t inventory_service = helper.getInstance(libUnity.start + SimpleSkinOffsets::inventoryService_addr, true, 0x0);
    if (!isValidPtr(inventory_service)) {
        inventory_service = helper.getInstance(libUnity.start + SimpleSkinOffsets::inventoryService_addr, false, 0x0);
    }
    if (!isValidPtr(inventory_service)) return;
    
    
    uint64_t skins_dict = mem.read<uint64_t>(inventory_service + SimpleSkinOffsets::inventorySkinsDict);
    if (!isValidPtr(skins_dict)) return;
    
    int count = UnityDict<int, uint64_t>::Count(skins_dict);
    if (count <= 0) return;
    
    
    static bool logged = false;
    if (!logged) {
        logged = true;
        for (int i = 0; i < count && i < 50; i++) {
            uint64_t skin = UnityDict<int, uint64_t>::GetValue(skins_dict, i);
            if (isValidPtr(skin)) {
                int skinID = mem.read<int>(skin + SimpleSkinOffsets::invItem_id);
                printf("Inventory skin %d: ID = %d\n", i, skinID);
            }
        }
    }
    
    
    int random_idx = rand() % count;
    uint64_t random_skin = UnityDict<int, uint64_t>::GetValue(skins_dict, random_idx);
    if (!isValidPtr(random_skin)) return;
    
    
    int selectedIndex = functions.selectedKnifeSkin;
    if (selectedIndex < 0 || selectedIndex >= sizeof(skinIDs) / sizeof(skinIDs[0])) {
        selectedIndex = 0; 
    }
    int realSkinID = skinIDs[selectedIndex];
    
    printf("Changing skin to ID: %d (index %d)\n", realSkinID, selectedIndex);
    
    
    mem.write<int>(random_skin + SimpleSkinOffsets::invItem_id, realSkinID);
}
