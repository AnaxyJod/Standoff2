#include "includes/uses.h"
#include "includes/players.h"
#include "includes/hitlog.h"
#include "includes/tracer.h"
#include "includes/hitmarker.h"
#include "includes/aim.h"
#include "includes/rcs.h"
#include "includes/silent.h"
#include "menu/ui.h"

uint64_t playerManager = 0;
uint64_t localPlayer = 0;
uint64_t photonPlayer = 0;

using namespace std;

int main(){
    
    pid = GetPID("com.axlebolt.standoff2");
    
    if (pid <= 0) {
        std::system("am force-stop com.axlebolt.standoff2");
        std::system("am start -n com.axlebolt.standoff2/com.google.firebase.MessagingUnityPlayerActivity");

        for (int i = 0; i < 100; ++i) { 
            usleep(100000);
            pid = GetPID("com.axlebolt.standoff2");
            if (pid > 0) break;
        }
    }
    
    if (pid <= 0)
        return 0;
    
    
    mem.setPid(pid);
    
    
    uintptr_t libUnityBase = 0;
    do {
        libUnityBase = GetModuleBase(pid, "libunity.so");
        usleep(100000);
    } while (libUnityBase == 0);
    
    
    libUnity = libraryInfo(libUnityBase, libUnityBase + 0x10000000);
  
    if (!draw::initialize(true))
        return -1;
    
    cout<<"pid : "<<pid<<endl;
    cout<<"libunity : 0x"<<hex<<libUnity.start<<endl;
    cout<<"playerManager : 0x"<<hex<<libUnity.start+offsets::playerManager_addr<<endl;
    
    while(true){
        draw::processInput();
        if (!ImGui::GetCurrentContext()) {
            usleep(20000);
            continue;
        }
        draw::beginFrame();
        if (!ImGui::GetCurrentContext()) {
            usleep(20000);
            continue;
        }

        menu.render();
        
        
        hitLogger.Update();
        hitMarker.Update();
        
        uint64_t playerManager = helper.getInstance(libUnity.start + offsets::playerManager_addr, true, 0x0);
        
        
        static uint64_t lastPlayerManager = 0;
        lastPlayerManager = playerManager;
        
        if(playerManager){
            uint64_t playersList = mem.read<uint64_t>(playerManager + offsets::entityList);
            uint64_t localPlayer = mem.read<uint64_t>(playerManager + offsets::localPlayer_);
            
            if (playersList && localPlayer){
                
                functions.viewMatrix = mem.read<Matrix>(mem.read<uint64_t>(mem.read<uint64_t>(mem.read<uint64_t>(localPlayer + offsets::viewMatrix_ptr1) + offsets::viewMatrix_ptr2) + offsets::viewMatrix_ptr3) + offsets::viewMatrix_ptr4);
                
                
                if (bulletTracer.enabled) {
                    Vector3 startPos, hitPoint;
                    if (CheckNewShot(localPlayer, startPos, hitPoint)) {
                        bulletTracer.AddTrace(startPos, hitPoint);
                    }
                    bulletTracer.Update();
                }
                
                
                
                std::string localPlayerName = players.getName(localPlayer);
                
                
                uint64_t localPhotonPlayer = getPhotonPlayer(localPlayer);
                
                functions.playerCount = mem.read<int>(playersList + offsets::entityList_count);
                for (int i = 0; i < functions.playerCount; i++){
                    uint64_t player = mem.read<uint64_t>(mem.read<uint64_t>(playersList + offsets::player_ptr1) + offsets::player_ptr2 + offsets::player_ptr3 * i);
                    
                    if(player!=0){
                        int team = players.getTeam(player);
                        int localTeam = players.getTeam(localPlayer);
                        if(team!=localTeam){
                            int health = players.getHealth(player);
                            int selfHealth = players.getHealth(localPlayer);
                            int armor = players.getArmor(player);
                            int money = players.getMoney(player);
                            int ping = players.getPing(player);
                            auto name = players.getName(player);
                            auto wpn = players.getWeaponName(player);
                            
                            
                            {
                                auto& state = playerHealthStates[player];
                                uint64_t lastAttacker = getLastAttacker(player);
                                
                                if (state.isValid && !name.empty() && state.lastHealth > 0) {
                                    int healthDiff = state.lastHealth - health;
                                    
                                    if (healthDiff > 0) {
                                        bool isLocalPlayerAttacker = (lastAttacker != 0 && lastAttacker == localPhotonPlayer);
                                        
                                        if (isLocalPlayerAttacker) {
                                            bool isKill = (health <= 0);
                                            std::string currentWeapon = players.getWeaponName(localPlayer);
                                            bool isHeadshot = (healthDiff >= GameData::GetHeadshotThreshold(currentWeapon));
                                            
                                            if (hitLogger.enabled) {
                                                hitLogger.LogHit(localPlayerName, name, healthDiff, currentWeapon, isHeadshot, isKill);
                                            }
                                            
                                            if (hitMarkerSettings.enabled) {
                                                Vector3 localPos = getPlayerPosition(localPlayer);
                                                localPos.y += 1.6f;
                                                
                                                Vector3 hitPoint = getHitPoint(player);
                                                if (hitPoint == Vector3::Zero()) {
                                                    hitPoint = getPlayerPosition(player);
                                                    hitPoint.y += 1.0f;
                                                }
                                                
                                                hitMarker.AddMarker(localPos, hitPoint, healthDiff, isHeadshot, isKill);
                                                
                                                
                                                if (bulletTracer.enabled && bulletTracer.stopOnHit) {
                                                    bulletTracer.UpdateLastTraceWithEnemyHit(hitPoint);
                                                }
                                            } else if (bulletTracer.enabled && bulletTracer.stopOnHit) {
                                                
                                                Vector3 hitPoint = getHitPoint(player);
                                                if (hitPoint == Vector3::Zero()) {
                                                    hitPoint = getPlayerPosition(player);
                                                    hitPoint.y += 1.0f;
                                                }
                                                bulletTracer.UpdateLastTraceWithEnemyHit(hitPoint);
                                            }
                                        }
                                    }
                                }
                                
                                state.lastHealth = health;
                                state.lastArmor = armor;
                                state.name = name;
                                state.isValid = true;
                            }
                            
                            
                            if(health <= 0) continue;
                        
                            Vector3 position = getPlayerPosition(player);
                            functions.viewMatrix = mem.read<Matrix>(mem.read<uint64_t>(mem.read<uint64_t>(mem.read<uint64_t>(localPlayer + offsets::viewMatrix_ptr1) + offsets::viewMatrix_ptr2) + offsets::viewMatrix_ptr3) + offsets::viewMatrix_ptr4);
                        
                            if(selfHealth>=0){
                                functions.esp.Update(position, health, team, localTeam, armor, ping, money, name, wpn, player, localPlayer);
                                functions.isAlive = true;
                            }
                            else
                                functions.isAlive = false;
                        }
                    }
                }
                
                
                
                HostIndicator();
                
                
                if (hitMarkerSettings.enabled) {
                    hitMarker.RenderMarkers(functions.viewMatrix);
                }
                
                
                if (bulletTracer.enabled) {
                    bulletTracer.Render(functions.viewMatrix);
                }
                
                
                if (hitMarkerSettings.enabled) {
                    hitMarker.RenderDamage(functions.viewMatrix);
                }
                
                
                aimbot.Render();
                
                
                silent.Render();
                
                
                recoilControl.Render();
                
                
                InfinityAmmo();
                MoneyHack();
                Wallshot();
                FireRateHack();
                NoRecoil();
                

                
            }
        }
        
        
        hitLogger.Render();
        
        if (ImGui::GetCurrentContext()) {
            draw::endFrame();
        }

        if (menu.should_exit)
            break;
        usleep(20000); 
    }
    
    draw::shutdown();
    return 0;
}
