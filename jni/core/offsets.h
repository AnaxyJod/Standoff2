#pragma once
#include <cstdint>

namespace Offsets {
    
    struct StaticAddresses {
        static constexpr int playerManager = 135621384;
        static constexpr int grenadeManager = 138151768;
        static constexpr int droppedWeaponManager = 134382512;
        static constexpr int photonNetwork = 135620632;
        static constexpr int bombManager = 138126120;
        static constexpr int recoilParameters = 135623800;
        static constexpr int gunParameters = 135605440;
        static constexpr int weaponParameters = 135635824;
        static constexpr int weaponManager = 135635776;
        static constexpr int boltInventoryService = 135588032;
    };
    
    struct Player {
        static constexpr int localPlayer = 0x70;
        static constexpr int entityList = 0x28;
        static constexpr int entityListCount = 0x20;
        static constexpr int photonPointer = 0x158;
        static constexpr int photonView = 0x148;
        static constexpr int team = 0x79;
        static constexpr int playerCharacterView = 0x48;
        static constexpr int weaponryController = 0x88;
        static constexpr int movementController = 0x98;
        static constexpr int armsAnimationController = 0xA0;
        static constexpr int playerOcclusionController = 0xB0;
        static constexpr int playerMainCamera = 0xE0;
        static constexpr int playerHitController = 0xA8;
        static constexpr int lastAttacker = 0xD0;
        
        static constexpr int playerPtr1 = 0x18;
        static constexpr int playerPtr2 = 0x10;
        static constexpr int playerPtr3 = 0x18;
    };
    
    struct Weapon {
        static constexpr int currentWeaponController = 0xA0;
        static constexpr int kitController = 0x98;
        static constexpr int weaponParameters = 0xA0;
        static constexpr int weaponParametersWallshot = 0xA8;
        static constexpr int weaponDisplayName = 0x98;
        static constexpr int weaponID = 0x18;
        
        static constexpr int ammoInMagazine = 0x110;
        static constexpr int reserveAmmo = 0x114;
        static constexpr int reserveAmmoStruct = 0x118;
        static constexpr int magazineAmmoStruct = 0x120;
        
        static constexpr int fireDelay = 0x100;
        static constexpr int gunParameters = 0x160;
        static constexpr int fireState = 0x140;
        static constexpr int shotCount = 0x110;
    };
    
    struct Photon {
        static constexpr int name = 0x20;
        static constexpr int properties = 0x38;
        static constexpr int actorNumber = 0x18;
        static constexpr int room = 0x160;
        static constexpr int roomMasterActorNumber = 0x48;
        static constexpr int managerPtr2 = 0xB8;
        static constexpr int photonNetworkPtr1 = 0x18;
        static constexpr int photonRoom = 0x170;
    };
    
    struct View {
        static constexpr int viewVisible = 0x30;
        static constexpr int bipedMap = 0x48;
        
        static constexpr uint64_t boneHead = 0x20;
        static constexpr uint64_t boneNeck = 0x28;
        static constexpr uint64_t boneSpine = 0x30;
        static constexpr uint64_t boneSpine1 = 0x38;
        static constexpr uint64_t boneSpine2 = 0x40;
        static constexpr uint64_t boneLeftShoulder = 0x48;
        static constexpr uint64_t boneLeftElbow = 0x50;
        static constexpr uint64_t boneLeftHand = 0x58;
        static constexpr uint64_t boneRightShoulder = 0x68;
        static constexpr uint64_t boneRightElbow = 0x70;
        static constexpr uint64_t boneRightHand = 0x78;
        static constexpr uint64_t boneHip = 0x88;
        static constexpr uint64_t boneLeftThigh = 0x90;
        static constexpr uint64_t boneLeftKnee = 0x98;
        static constexpr uint64_t boneLeftFoot = 0xA0;
        static constexpr uint64_t boneRightThigh = 0xB0;
        static constexpr uint64_t boneRightKnee = 0xB8;
        static constexpr uint64_t boneRightFoot = 0xC0;
    };
    
    struct ViewMatrix {
        static constexpr int cameraPtr = 0xE0;
        static constexpr int transformPtr = 0x20;
        static constexpr int cameraObject = 0x10;
        static constexpr int matrix = 0x100;
        static constexpr int fov = 0x180;
        static constexpr int aspectRatio = 0x4F0;
        static constexpr int cameraScopeZoomer = 0x28;
        static constexpr int scopeFov = 0x38;
    };
    
    struct Grenade {
        static constexpr int dict = 0x28;
        static constexpr int transform = 0x40;
        static constexpr int grenadeID = 0x48;
        static constexpr int state = 0x4C;
        static constexpr int detonationType = 0x50;
    };
    
    struct DroppedWeapon {
        static constexpr int dict = 0x30;
        static constexpr int id = 0x80;
        static constexpr int transform = 0x90;
        static constexpr int data = 0xA8;
        static constexpr int params = 0x30;
        static constexpr int ammunition = 0x130;
        static constexpr int maxAmmo = 0x10;
        static constexpr int currentAmmo = 0x50;
    };
    
    struct AimController {
        static constexpr int aimData = 0x90;
        
        static constexpr int pitch = 0x18;
        static constexpr int yaw = 0x1C;
        static constexpr int targetPitch = 0x24;
        static constexpr int targetYaw = 0x28;
    };
    
    struct Occlusion {
        static constexpr int controller = 0xB0;
        static constexpr int visibilityState = 0x34;
        static constexpr int occlusionState = 0x38;
    };
    
    struct WeaponManager {
        static constexpr int weaponManager = 0xB8;
        static constexpr int weaponCache = 0x60;
        static constexpr int weaponParamsArray = 0x50;
        
        static constexpr int weaponAvailable = 0x48;
        static constexpr int weaponLocked = 0x4C;
        static constexpr int weaponPrice = 0x50;
    };
    
    struct HitData {
        static constexpr int lastHitData = 0xE8;
        static constexpr int hitsArray = 0x38;
        static constexpr int hitsCount = 0x18;
        static constexpr int firstHit = 0x20;
        static constexpr int hitPoint = 0x1C;
        static constexpr int hitPointAlt = 0x10;
    };
    
    struct Inventory {
        static constexpr int weaponDict = 0x58;
        static constexpr int kitController = 0x98;
    };
    
    struct Movement {
        static constexpr int translationParameters = 0xA8;
        static constexpr int transformData = 0xB0;
        static constexpr int character = 0xF0;
        static constexpr int position = 0x44;
    };
    
    struct PhotonView {
        static constexpr int lagState = 0x50;
    };
    
    struct ArmsAnimation {
        static constexpr int viewmodelOffset = 0xE8;
    };
    
    struct Bomb {
        static constexpr int bombController = 0x28;
        static constexpr int transform = 0x40;
        static constexpr int isPlanted = 0x48;
        static constexpr int isDefused = 0x49;
        static constexpr int isDetonated = 0x4A;
        static constexpr int timer = 0x4C;
    };
}

namespace offsets {
    inline int playerManager_addr = Offsets::StaticAddresses::playerManager;
    inline int grenadeManager_addr = Offsets::StaticAddresses::grenadeManager;
    inline int droppedWeaponManager_addr = Offsets::StaticAddresses::droppedWeaponManager;
    inline int PhotonNetwork_addr = Offsets::StaticAddresses::photonNetwork;
    inline int bombManager_addr = Offsets::StaticAddresses::bombManager;
    
    inline int localPlayer_ = Offsets::Player::localPlayer;
    inline int entityList = Offsets::Player::entityList;
    inline int entityList_count = Offsets::Player::entityListCount;
    inline int photonPointer = Offsets::Player::photonPointer;
    inline int photonView = Offsets::Player::photonView;
    inline int team = Offsets::Player::team;
    inline int playerCharacterView = Offsets::Player::playerCharacterView;
    inline int weaponryController = Offsets::Player::weaponryController;
    inline int movementController = Offsets::Player::movementController;
    inline int playerHitController = Offsets::Player::playerHitController;
    inline int lastAttacker = Offsets::Player::lastAttacker;
    
    inline int player_ptr1 = Offsets::Player::playerPtr1;
    inline int player_ptr2 = Offsets::Player::playerPtr2;
    inline int player_ptr3 = Offsets::Player::playerPtr3;
    
    inline int currentWeaponController = Offsets::Weapon::currentWeaponController;
    inline int weaponParameters = Offsets::Weapon::weaponParameters;
    inline int weaponDisplayName = Offsets::Weapon::weaponDisplayName;
    
    inline int name = Offsets::Photon::name;
    inline int photonProperties = Offsets::Photon::properties;
    inline int photonActorNumber = Offsets::Photon::actorNumber;
    inline int room = Offsets::Photon::room;
    inline int roomMasterActorNumber = Offsets::Photon::roomMasterActorNumber;
    inline int manager_ptr2() { return Offsets::Photon::managerPtr2; }
    inline int photonnetwork_ptr1() { return Offsets::Photon::photonNetworkPtr1; }
    inline int photon_room() { return Offsets::Photon::photonRoom; }
    
    inline int viewVisible = Offsets::View::viewVisible;
    inline int bipedMap = Offsets::View::bipedMap;
    inline uint64_t bone_head = Offsets::View::boneHead;
    inline uint64_t bone_neck = Offsets::View::boneNeck;
    inline uint64_t bone_spine = Offsets::View::boneSpine;
    inline uint64_t bone_leftShoulder = Offsets::View::boneLeftShoulder;
    inline uint64_t bone_leftUpperArm = Offsets::View::boneLeftElbow;
    inline uint64_t bone_leftHand = Offsets::View::boneLeftHand;
    inline uint64_t bone_rightShoulder = Offsets::View::boneRightShoulder;
    inline uint64_t bone_rightUpperArm = Offsets::View::boneRightElbow;
    inline uint64_t bone_rightHand = Offsets::View::boneRightHand;
    inline uint64_t bone_hip = Offsets::View::boneHip;
    inline uint64_t bone_leftUpperLeg = Offsets::View::boneLeftKnee;
    inline uint64_t bone_leftLowerLeg = Offsets::View::boneLeftFoot;
    inline uint64_t bone_rightUpperLeg = Offsets::View::boneRightKnee;
    inline uint64_t bone_rightLowerLeg = Offsets::View::boneRightFoot;
    
    inline int viewMatrix_ptr1 = Offsets::ViewMatrix::cameraPtr;
    inline int viewMatrix_ptr2 = Offsets::ViewMatrix::transformPtr;
    inline int viewMatrix_ptr3 = Offsets::ViewMatrix::cameraObject;
    inline int viewMatrix_ptr4 = Offsets::ViewMatrix::matrix;
    
    inline int grenadeDict = Offsets::Grenade::dict;
    inline int grenadeTransform = Offsets::Grenade::transform;
    inline int grenadeID = Offsets::Grenade::grenadeID;
    inline int grenadeStates = Offsets::Grenade::state;
    inline int grenadeDetonationType = Offsets::Grenade::detonationType;
    
    inline int droppedWeaponDict = Offsets::DroppedWeapon::dict;
    inline int droppedWeaponID = Offsets::DroppedWeapon::id;
    inline int droppedWeaponTransform = Offsets::DroppedWeapon::transform;
    inline int droppedWeaponData = Offsets::DroppedWeapon::data;
    inline int droppedWeaponParams = Offsets::DroppedWeapon::params;
    inline int droppedWeaponAmmunition = Offsets::DroppedWeapon::ammunition;
    inline int droppedWeaponMaxAmmo = Offsets::DroppedWeapon::maxAmmo;
    inline int droppedWeaponCurrentAmmo = Offsets::DroppedWeapon::currentAmmo;
}

namespace aimOffsets {
    inline uint64_t bone_head = Offsets::View::boneHead;
    inline uint64_t bone_neck = Offsets::View::boneNeck;
    inline uint64_t bone_spine = Offsets::View::boneSpine;
    inline uint64_t bone_hip = Offsets::View::boneHip;
    inline uint64_t bone_leftShoulder = Offsets::View::boneLeftShoulder;
    inline uint64_t bone_leftUpperArm = Offsets::View::boneLeftElbow;
    inline uint64_t bone_leftHand = Offsets::View::boneLeftHand;
    inline uint64_t bone_rightShoulder = Offsets::View::boneRightShoulder;
    inline uint64_t bone_rightUpperArm = Offsets::View::boneRightElbow;
    inline uint64_t bone_rightHand = Offsets::View::boneRightHand;
    inline uint64_t bone_leftUpperLeg = Offsets::View::boneLeftKnee;
    inline uint64_t bone_leftLowerLeg = Offsets::View::boneLeftFoot;
    inline uint64_t bone_rightUpperLeg = Offsets::View::boneRightKnee;
    inline uint64_t bone_rightLowerLeg = Offsets::View::boneRightFoot;
    
    inline int playerCharacterView = Offsets::Player::playerCharacterView;
    inline int bipedMap = Offsets::View::bipedMap;
    inline int aimData = Offsets::AimController::aimData;
}
