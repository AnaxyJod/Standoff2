#include "includes/cfg_manager.h"
#include "includes/uses.h" 
#include "includes/hitlog.h"
#include "includes/tracer.h"
#include "includes/hitmarker.h"
#include "includes/aim.h"
#include "includes/rcs.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>

bool ConfigManager::SaveConfig(const std::string& filepath) {
    
    std::string directory = GetDirectoryFromPath(filepath);
    if (!directory.empty() && !CreateDirectoryIfNotExists(directory)) {
        std::cerr << "Failed to create directory for: " << filepath << std::endl;
        return false;
    }

    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file for writing: " << filepath << std::endl;
        std::cerr << "Please check if the path exists and you have write permissions" << std::endl;
        return false;
    }

    
    file << "esp_enabled " << (functions.esp.enabled ? "1" : "0") << "\n";
    file << "esp_box " << (functions.esp.box ? "1" : "0") << "\n";
    file << "esp_box_filled " << (functions.esp.box_filled ? "1" : "0") << "\n";
    file << "esp_tracer " << (functions.esp.tracer ? "1" : "0") << "\n";
    file << "esp_tracer_start_pos " << functions.esp.tracer_start_pos << "\n";
    file << "esp_tracer_end_pos " << functions.esp.tracer_end_pos << "\n";
    file << "esp_offscreen " << (functions.esp.offscreen ? "1" : "0") << "\n";
    file << "esp_offscreen_force " << (functions.esp.offscreen_force ? "1" : "0") << "\n";
    file << "esp_offscreen_radius " << functions.esp.offscreen_radius << "\n";
    file << "esp_offscreen_size " << functions.esp.offscreen_size << "\n";
    file << "esp_visible_check " << (functions.esp.visible_check ? "1" : "0") << "\n";
    file << "esp_bomber " << (functions.esp.bomber ? "1" : "0") << "\n";
    file << "esp_bomber_offset " << functions.esp.bomber_offset << "\n";
    file << "esp_bomber_position " << functions.esp.bomber_position << "\n";
    file << "esp_defuser " << (functions.esp.defuser ? "1" : "0") << "\n";
    file << "esp_defuser_offset " << functions.esp.defuser_offset << "\n";
    file << "esp_defuser_position " << functions.esp.defuser_position << "\n";
    file << "esp_skeleton " << (functions.esp.skeleton ? "1" : "0") << "\n";
    file << "esp_footstep " << (functions.esp.footstep ? "1" : "0") << "\n";
    file << "esp_health_bar " << (functions.esp.health_bar ? "1" : "0") << "\n";
    file << "esp_armor_bar " << (functions.esp.armor_bar ? "1" : "0") << "\n";
    file << "esp_name " << (functions.esp.name ? "1" : "0") << "\n";
    file << "esp_host " << (functions.esp.host ? "1" : "0") << "\n";
    file << "esp_weapon " << (functions.esp.weapon ? "1" : "0") << "\n";
    file << "esp_weapon_text " << (functions.esp.weapon_text ? "1" : "0") << "\n";
    file << "esp_grenades_view " << (functions.esp.greandes_view ? "1" : "0") << "\n";
    file << "esp_dropped_weapon " << (functions.esp.dropped_weapon ? "1" : "0") << "\n";
    file << "custom_cross " << (functions.esp.custom_cross ? "1" : "0") << "\n";
    file << "night_mode " << (functions.esp.night_mode ? "1" : "0") << "\n";
    file << "esp_health_text " << (functions.esp.health_text ? "1" : "0") << "\n";
    file << "esp_armor_text " << (functions.esp.armor_text ? "1" : "0") << "\n";
    
    
    file << "esp_box_type " << functions.esp.box_type << "\n";
    file << "esp_corner_length " << functions.esp.corner_length << "\n";
    file << "esp_round " << functions.esp.round << "\n";
    file << "esp_box_width_ratio " << functions.esp.box_width_ratio << "\n";
    file << "esp_box3d_rot " << (functions.esp.box3d_rot ? "1" : "0") << "\n";
    file << "esp_box3d_ray " << (functions.esp.box3d_ray ? "1" : "0") << "\n";

    
    file << "esp_flags_money " << (functions.esp.flags.money ? "1" : "0") << "\n";
    file << "esp_flags_hk " << (functions.esp.flags.hk ? "1" : "0") << "\n";
    file << "esp_flags_ping " << (functions.esp.flags.ping ? "1" : "0") << "\n";
    file << "esp_flags_distance " << (functions.esp.flags.distance ? "1" : "0") << "\n";
    file << "esp_flags_offset " << functions.esp.flags.offset << "\n";
    file << "esp_flags_position " << functions.esp.flags.position << "\n";
    file << "esp_flags_order " << functions.esp.flags.order[0] << " " << functions.esp.flags.order[1] << " " << functions.esp.flags.order[2] << "\n";
    file << "esp_flags_nextOrder " << functions.esp.flags.nextOrder << "\n";

    
    WriteFloatArray(file, "esp_box_color", functions.esp.box_color, 4);
    WriteFloatArray(file, "esp_tracer_color", functions.esp.tracer_color, 4);
    WriteFloatArray(file, "esp_offscreen_color", functions.esp.offscreen_color, 4);
    WriteFloatArray(file, "esp_bomber_color", functions.esp.bomber_color, 4);
    WriteFloatArray(file, "esp_defuser_color", functions.esp.defuser_color, 4);
    WriteFloatArray(file, "esp_weapon_color", functions.esp.weapon_color, 4);
    WriteFloatArray(file, "esp_weapon_color_text", functions.esp.weapon_color_text, 4);
    WriteFloatArray(file, "esp_name_color", functions.esp.name_color, 4);
    WriteFloatArray(file, "esp_skeleton_color", functions.esp.skeleton_color, 4);
    WriteFloatArray(file, "esp_footstep_color", functions.esp.footstep_color, 4);
    WriteFloatArray(file, "esp_health_bar_color", functions.esp.health_bar_color, 4);
    WriteFloatArray(file, "esp_armor_bar_color", functions.esp.armor_bar_color, 4);
    WriteFloatArray(file, "esp_filled_color", functions.esp.filled_color, 4);
    WriteFloatArray(file, "esp_distance_color", functions.esp.distance_color, 4);
    WriteFloatArray(file, "esp_money_color", functions.esp.money_color, 4);
    WriteFloatArray(file, "esp_ping_color", functions.esp.ping_color, 4);
    WriteFloatArray(file, "esp_dropped_weapon_color", functions.esp.dropped_weapon_color, 4);
    WriteFloatArray(file, "cross_color", functions.esp.cross_color, 4);
    
    
    WriteFloatArray(file, "esp_box_color_visible", functions.esp.box_color_visible, 4);
    WriteFloatArray(file, "esp_tracer_color_visible", functions.esp.tracer_color_visible, 4);
    WriteFloatArray(file, "esp_offscreen_color_visible", functions.esp.offscreen_color_visible, 4);
    WriteFloatArray(file, "esp_bomber_color_visible", functions.esp.bomber_color_visible, 4);
    WriteFloatArray(file, "esp_defuser_color_visible", functions.esp.defuser_color_visible, 4);
    WriteFloatArray(file, "esp_weapon_color_visible", functions.esp.weapon_color_visible, 4);
    WriteFloatArray(file, "esp_weapon_color_text_visible", functions.esp.weapon_color_text_visible, 4);
    WriteFloatArray(file, "esp_name_color_visible", functions.esp.name_color_visible, 4);
    WriteFloatArray(file, "esp_skeleton_color_visible", functions.esp.skeleton_color_visible, 4);
    WriteFloatArray(file, "esp_footstep_color_visible", functions.esp.footstep_color_visible, 4);
    WriteFloatArray(file, "esp_health_bar_color_visible", functions.esp.health_bar_color_visible, 4);
    WriteFloatArray(file, "esp_armor_bar_color_visible", functions.esp.armor_bar_color_visible, 4);
    WriteFloatArray(file, "esp_filled_color_visible", functions.esp.filled_color_visible, 4);
    WriteFloatArray(file, "esp_distance_color_visible", functions.esp.distance_color_visible, 4);
    WriteFloatArray(file, "esp_money_color_visible", functions.esp.money_color_visible, 4);
    WriteFloatArray(file, "esp_ping_color_visible", functions.esp.ping_color_visible, 4);
    WriteFloatArray(file, "esp_dropped_weapon_color_visible", functions.esp.dropped_weapon_color_visible, 4);
    
    
    file << "rainbow_box " << (functions.esp.rainbow_box ? "1" : "0") << "\n";
    file << "rainbow_filled " << (functions.esp.rainbow_filled ? "1" : "0") << "\n";
    file << "rainbow_footstep " << (functions.esp.rainbow_footstep ? "1" : "0") << "\n";
    file << "rainbow_skeleton " << (functions.esp.rainbow_skeleton ? "1" : "0") << "\n";
    file << "rainbow_tracer " << (functions.esp.rainbow_tracer ? "1" : "0") << "\n";
    file << "rainbow_offscreen " << (functions.esp.rainbow_offscreen ? "1" : "0") << "\n";
    file << "rainbow_health_bar " << (functions.esp.rainbow_health_bar ? "1" : "0") << "\n";
    file << "rainbow_armor_bar " << (functions.esp.rainbow_armor_bar ? "1" : "0") << "\n";
    file << "rainbow_name " << (functions.esp.rainbow_name ? "1" : "0") << "\n";
    file << "rainbow_distance " << (functions.esp.rainbow_distance ? "1" : "0") << "\n";
    file << "rainbow_money " << (functions.esp.rainbow_money ? "1" : "0") << "\n";
    file << "rainbow_ping " << (functions.esp.rainbow_ping ? "1" : "0") << "\n";
    file << "rainbow_bomber " << (functions.esp.rainbow_bomber ? "1" : "0") << "\n";
    file << "rainbow_defuser " << (functions.esp.rainbow_defuser ? "1" : "0") << "\n";
    file << "rainbow_weapon " << (functions.esp.rainbow_weapon ? "1" : "0") << "\n";
    file << "rainbow_weapon_text " << (functions.esp.rainbow_weapon_text ? "1" : "0") << "\n";
    file << "rainbow_dropped_weapon " << (functions.esp.rainbow_dropped_weapon ? "1" : "0") << "\n";
    
    
    file << "rainbow_box_visible " << (functions.esp.rainbow_box_visible ? "1" : "0") << "\n";
    file << "rainbow_filled_visible " << (functions.esp.rainbow_filled_visible ? "1" : "0") << "\n";
    file << "rainbow_footstep_visible " << (functions.esp.rainbow_footstep_visible ? "1" : "0") << "\n";
    file << "rainbow_skeleton_visible " << (functions.esp.rainbow_skeleton_visible ? "1" : "0") << "\n";
    file << "rainbow_tracer_visible " << (functions.esp.rainbow_tracer_visible ? "1" : "0") << "\n";
    file << "rainbow_offscreen_visible " << (functions.esp.rainbow_offscreen_visible ? "1" : "0") << "\n";
    file << "rainbow_health_bar_visible " << (functions.esp.rainbow_health_bar_visible ? "1" : "0") << "\n";
    file << "rainbow_armor_bar_visible " << (functions.esp.rainbow_armor_bar_visible ? "1" : "0") << "\n";
    file << "rainbow_name_visible " << (functions.esp.rainbow_name_visible ? "1" : "0") << "\n";
    file << "rainbow_distance_visible " << (functions.esp.rainbow_distance_visible ? "1" : "0") << "\n";
    file << "rainbow_money_visible " << (functions.esp.rainbow_money_visible ? "1" : "0") << "\n";
    file << "rainbow_ping_visible " << (functions.esp.rainbow_ping_visible ? "1" : "0") << "\n";
    file << "rainbow_bomber_visible " << (functions.esp.rainbow_bomber_visible ? "1" : "0") << "\n";
    file << "rainbow_defuser_visible " << (functions.esp.rainbow_defuser_visible ? "1" : "0") << "\n";
    file << "rainbow_weapon_visible " << (functions.esp.rainbow_weapon_visible ? "1" : "0") << "\n";
    file << "rainbow_weapon_text_visible " << (functions.esp.rainbow_weapon_text_visible ? "1" : "0") << "\n";
    file << "rainbow_dropped_weapon_visible " << (functions.esp.rainbow_dropped_weapon_visible ? "1" : "0") << "\n";
    WriteFloat(file, "rainbow_speed", functions.esp.rainbow_speed);
    
    WriteFloat(file, "esp_footstep_delta", functions.esp.footstep_delta);
    WriteFloat(file, "esp_footstep_speed", functions.esp.footstep_speed);
    
    
    WriteFloat(file, "esp_box_width", functions.esp.box_width);
    WriteFloat(file, "esp_box_outline_width", functions.esp.box_outline_width);
    file << "esp_box_outline_enabled " << (functions.esp.box_outline_enabled ? "1" : "0") << "\n";
    WriteFloat(file, "esp_skeleton_width", functions.esp.skeleton_width);
    WriteFloat(file, "esp_tracer_width", functions.esp.tracer_width);
    WriteFloat(file, "esp_health_bar_width", functions.esp.health_bar_width);
    file << "esp_health_bar_outline_enabled " << (functions.esp.health_bar_outline_enabled ? "1" : "0") << "\n";
    WriteFloat(file, "esp_armor_bar_width", functions.esp.armor_bar_width);
    file << "esp_armor_bar_outline_enabled " << (functions.esp.armor_bar_outline_enabled ? "1" : "0") << "\n";
    WriteFloat(file, "esp_health_bar_offset", functions.esp.health_bar_offset);
    WriteFloat(file, "esp_health_bar_position_offset", functions.esp.health_bar_position_offset);
    WriteFloat(file, "esp_armor_bar_offset", functions.esp.armor_bar_offset);
    WriteFloat(file, "esp_name_offset", functions.esp.name_offset);
    WriteFloat(file, "esp_weapon_name_offset", functions.esp.weapon_name_offset);
    WriteFloat(file, "esp_weapon_icon_offset", functions.esp.weapon_icon_offset);
    WriteFloat(file, "esp_weapon_icon_size", functions.esp.weapon_icon_size);
    
    
    file << "esp_health_bar_position " << functions.esp.health_bar_position << "\n";
    file << "esp_armor_bar_position " << functions.esp.armor_bar_position << "\n";
    file << "esp_name_position " << functions.esp.name_position << "\n";
    file << "esp_weapon_name_position " << functions.esp.weapon_name_position << "\n";

    
    file << "hitlog_enabled " << (hitLogger.enabled ? "1" : "0") << "\n";
    file << "hitlog_show_headshot " << (hitLogger.showHeadshotIcon ? "1" : "0") << "\n";
    file << "hitlog_show_kill " << (hitLogger.showKillIcon ? "1" : "0") << "\n";
    file << "hitlog_show_weapon " << (hitLogger.showWeapon ? "1" : "0") << "\n";
    file << "hitlog_show_damage " << (hitLogger.showDamage ? "1" : "0") << "\n";
    WriteFloat(file, "hitlog_display_time", hitLogger.displayTime);
    WriteFloat(file, "hitlog_fade_delay", hitLogger.fadeDelay);
    file << "hitlog_max_entries " << hitLogger.maxEntries << "\n";
    WriteFloat(file, "hitlog_position_x", hitLogger.positionX);
    WriteFloat(file, "hitlog_position_y", hitLogger.positionY);
    WriteFloatArray(file, "hitlog_text_color", hitLogger.textColor, 4);
    WriteFloatArray(file, "hitlog_damage_color", hitLogger.damageColor, 4);
    WriteFloatArray(file, "hitlog_headshot_color", hitLogger.headshotColor, 4);
    WriteFloatArray(file, "hitlog_kill_color", hitLogger.killColor, 4);
    WriteFloatArray(file, "hitlog_weapon_color", hitLogger.weaponColor, 4);
    WriteFloatArray(file, "hitlog_bg_color", hitLogger.backgroundColor, 4);

    
    file << "tracer_enabled " << (bulletTracer.enabled ? "1" : "0") << "\n";
    WriteFloat(file, "tracer_display_time", bulletTracer.displayTime);
    WriteFloat(file, "tracer_fade_time", bulletTracer.fadeTime);
    WriteFloat(file, "tracer_line_thickness", bulletTracer.lineThickness);
    file << "tracer_use_gradient " << (bulletTracer.useGradient ? "1" : "0") << "\n";
    file << "tracer_stop_on_hit " << (bulletTracer.stopOnHit ? "1" : "0") << "\n";
    WriteFloatArray(file, "tracer_color1", bulletTracer.color1, 4);
    WriteFloatArray(file, "tracer_color2", bulletTracer.color2, 4);
    file << "tracer_show_hit_point " << (bulletTracer.showHitPoint ? "1" : "0") << "\n";
    WriteFloat(file, "tracer_hit_point_radius", bulletTracer.hitPointRadius);
    WriteFloatArray(file, "tracer_hit_point_color", bulletTracer.hitPointColor, 4);
    file << "tracer_max_traces " << bulletTracer.maxTraces << "\n";

    
    file << "hitmarker_enabled " << (hitMarkerSettings.enabled ? "1" : "0") << "\n";
    file << "hitmarker_type " << hitMarkerSettings.markerType << "\n";
    WriteFloat(file, "hitmarker_display_time", hitMarkerSettings.displayTime);
    WriteFloat(file, "hitmarker_fade_time", hitMarkerSettings.fadeTime);
    file << "hitmarker_use_hit_type_colors " << (hitMarkerSettings.useHitTypeColors ? "1" : "0") << "\n";
    WriteFloatArray(file, "hitmarker_hit_color", hitMarkerSettings.hitColor, 4);
    WriteFloatArray(file, "hitmarker_headshot_color", hitMarkerSettings.headshotColor, 4);
    WriteFloatArray(file, "hitmarker_kill_color", hitMarkerSettings.killColor, 4);
    file << "hitmarker_max_markers " << hitMarkerSettings.maxMarkers << "\n";

    
    file << "aim_enabled " << (aimSettings.enabled ? "1" : "0") << "\n";
    file << "aim_draw_fov " << (aimSettings.drawFov ? "1" : "0") << "\n";
    file << "aim_draw_fov_outline " << (aimSettings.drawFovOutline ? "1" : "0") << "\n";
    file << "aim_fov_check " << (aimSettings.fovCheck ? "1" : "0") << "\n";
    file << "aim_check_visible " << (aimSettings.checkVisible ? "1" : "0") << "\n";
    file << "aim_triggerbot " << (aimSettings.triggerBot ? "1" : "0") << "\n";
    file << "aim_fire_check " << (aimSettings.fireCheck ? "1" : "0") << "\n";
    WriteFloat(file, "aim_fov", aimSettings.fov);
    WriteFloat(file, "aim_smooth", aimSettings.smooth);
    WriteFloat(file, "aim_trigger_delay", aimSettings.triggerDelay);
    WriteFloat(file, "aim_max_distance", aimSettings.maxDistance);
    WriteFloat(file, "aim_target_switch_delay", aimSettings.targetSwitchDelay);
    WriteFloat(file, "aim_kill_switch_delay", aimSettings.killSwitchDelay);
    file << "aim_selected_bones";
    for (int i = 0; i < 14; i++) {
        file << " " << (aimSettings.selectedBones[i] ? 1 : 0);
    }
    file << "\n";
    file << "aim_hit_chance " << aimSettings.hitChance << "\n";
    WriteFloatArray(file, "aim_fov_color", aimSettings.fovColor, 4);

    
    file << "recoil_enabled " << (recoilSettings.enabled ? "1" : "0") << "\n";
    file << "recoil_horizontal " << recoilSettings.horizontalValue << "\n";
    file << "recoil_vertical " << recoilSettings.verticalValue << "\n";
    file << "recoil_no_spread " << (recoilSettings.noSpread ? "1" : "0") << "\n";



    file.close();
    std::cout << "Config saved successfully: " << filepath << std::endl;
    return true;
}

bool ConfigManager::LoadConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file for reading: " << filepath << std::endl;
        std::cerr << "File doesn't exist or no read permissions" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        
        if (key == "esp_enabled") {
            int value;
            iss >> value;
            functions.esp.enabled = (value != 0);
        }
        else if (key == "esp_box") {
            int value;
            iss >> value;
            functions.esp.box = (value != 0);
        }
        else if (key == "esp_box_filled") {
            int value;
            iss >> value;
            functions.esp.box_filled = (value != 0);
        }
        else if (key == "esp_tracer") {
            int value;
            iss >> value;
            functions.esp.tracer = (value != 0);
        }
        else if (key == "esp_tracer_start_pos") {
            iss >> functions.esp.tracer_start_pos;
        }
        else if (key == "esp_tracer_end_pos") {
            iss >> functions.esp.tracer_end_pos;
        }
        else if (key == "esp_offscreen") {
            int value;
            iss >> value;
            functions.esp.offscreen = (value != 0);
        }
        else if (key == "esp_offscreen_force") {
            int value;
            iss >> value;
            functions.esp.offscreen_force = (value != 0);
        }
        else if (key == "esp_offscreen_radius") {
            iss >> functions.esp.offscreen_radius;
        }
        else if (key == "esp_offscreen_size") {
            iss >> functions.esp.offscreen_size;
        }
        else if (key == "esp_visible_check") {
            int value;
            iss >> value;
            functions.esp.visible_check = (value != 0);
        }
        else if (key == "esp_bomber") {
            int value;
            iss >> value;
            functions.esp.bomber = (value != 0);
        }
        else if (key == "esp_bomber_offset") {
            iss >> functions.esp.bomber_offset;
        }
        else if (key == "esp_bomber_position") {
            iss >> functions.esp.bomber_position;
        }
        else if (key == "esp_defuser") {
            int value;
            iss >> value;
            functions.esp.defuser = (value != 0);
        }
        else if (key == "esp_defuser_offset") {
            iss >> functions.esp.defuser_offset;
        }
        else if (key == "esp_defuser_position") {
            iss >> functions.esp.defuser_position;
        }
        else if (key == "esp_skeleton") {
            int value;
            iss >> value;
            functions.esp.skeleton = (value != 0);
        }
        else if (key == "esp_footstep") {
            int value;
            iss >> value;
            functions.esp.footstep = (value != 0);
        }
        else if (key == "esp_health_bar") {
            int value;
            iss >> value;
            functions.esp.health_bar = (value != 0);
        }
        else if (key == "esp_armor_bar") {
            int value;
            iss >> value;
            functions.esp.armor_bar = (value != 0);
        }
        else if (key == "esp_name") {
            int value;
            iss >> value;
            functions.esp.name = (value != 0);
        }
        else if (key == "esp_host") {
            int value;
            iss >> value;
            functions.esp.host = (value != 0);
        }
        else if (key == "esp_weapon") {
            int value;
            iss >> value;
            functions.esp.weapon = (value != 0);
        }
        else if (key == "esp_weapon_text") {
            int value;
            iss >> value;
            functions.esp.weapon_text = (value != 0);
        }
        else if (key == "esp_grenades_view") {
            int value;
            iss >> value;
            functions.esp.greandes_view = (value != 0);
        }
        else if (key == "esp_dropped_weapon") {
            int value;
            iss >> value;
            functions.esp.dropped_weapon = (value != 0);
        }
        else if (key == "custom_cross") {
            int value;
            iss >> value;
            functions.esp.custom_cross = (value != 0);
        }
        else if (key == "night_mode") {
            int value;
            iss >> value;
            functions.esp.night_mode = (value != 0);
        }
        else if (key == "esp_health_text") {
            int value;
            iss >> value;
            functions.esp.health_text = (value != 0);
        }
        else if (key == "esp_armor_text") {
            int value;
            iss >> value;
            functions.esp.armor_text = (value != 0);
        }
        
        else if (key == "esp_box_type") {
            int value;
            iss >> value;
            functions.esp.box_type = value;
        }
        else if (key == "esp_corner_length") {
            iss >> functions.esp.corner_length;
        }
        else if (key == "esp_round") {
            iss >> functions.esp.round;
        }
        else if (key == "esp_box_width_ratio") {
            iss >> functions.esp.box_width_ratio;
        }
        else if (key == "esp_box3d") {
            
            int value;
            iss >> value;
            if (value != 0) functions.esp.box_type = 2;  
        }
        else if (key == "esp_box3d_rot") {
            int value;
            iss >> value;
            functions.esp.box3d_rot = (value != 0);
        }
        else if (key == "esp_box3d_ray") {
            int value;
            iss >> value;
            functions.esp.box3d_ray = (value != 0);
        }

        
        else if (key == "esp_flags_money") {
            int value;
            iss >> value;
            functions.esp.flags.money = (value != 0);
        }
        else if (key == "esp_flags_hk") {
            int value;
            iss >> value;
            functions.esp.flags.hk = (value != 0);
        }
        else if (key == "esp_flags_ping") {
            int value;
            iss >> value;
            functions.esp.flags.ping = (value != 0);
        }
        else if (key == "esp_flags_distance") {
            int value;
            iss >> value;
            functions.esp.flags.distance = (value != 0);
        }
        else if (key == "esp_flags_offset") {
            iss >> functions.esp.flags.offset;
        }
        else if (key == "esp_flags_position") {
            iss >> functions.esp.flags.position;
        }
        else if (key == "esp_flags_order") {
            iss >> functions.esp.flags.order[0] >> functions.esp.flags.order[1] >> functions.esp.flags.order[2];
        }
        else if (key == "esp_flags_nextOrder") {
            iss >> functions.esp.flags.nextOrder;
        }

        
        else if (key == "esp_box_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.box_color[i];
            }
        }
        else if (key == "esp_tracer_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.tracer_color[i];
            }
        }
        else if (key == "esp_offscreen_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.offscreen_color[i];
            }
        }
        else if (key == "esp_bomber_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.bomber_color[i];
            }
        }
        else if (key == "esp_defuser_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.defuser_color[i];
            }
        }
        else if (key == "esp_weapon_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.weapon_color[i];
            }
        }
        else if (key == "esp_weapon_color_text") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.weapon_color_text[i];
            }
        }
        else if (key == "esp_name_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.name_color[i];
            }
        }
        else if (key == "esp_skeleton_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.skeleton_color[i];
            }
        }
        else if (key == "esp_footstep_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.footstep_color[i];
            }
        }
        else if (key == "esp_health_bar_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.health_bar_color[i];
            }
        }
        else if (key == "esp_armor_bar_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.armor_bar_color[i];
            }
        }
        else if (key == "esp_filled_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.filled_color[i];
            }
        }
        else if (key == "esp_distance_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.distance_color[i];
            }
        }
        else if (key == "esp_money_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.money_color[i];
            }
        }
        else if (key == "esp_ping_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.ping_color[i];
            }
        }
        else if (key == "esp_dropped_weapon_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.dropped_weapon_color[i];
            }
        }
        else if (key == "cross_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.cross_color[i];
            }
        }
        
        else if (key == "esp_box_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.box_color_visible[i];
            }
        }
        else if (key == "esp_tracer_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.tracer_color_visible[i];
            }
        }
        else if (key == "esp_offscreen_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.offscreen_color_visible[i];
            }
        }
        else if (key == "esp_bomber_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.bomber_color_visible[i];
            }
        }
        else if (key == "esp_defuser_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.defuser_color_visible[i];
            }
        }
        else if (key == "esp_weapon_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.weapon_color_visible[i];
            }
        }
        else if (key == "esp_weapon_color_text_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.weapon_color_text_visible[i];
            }
        }
        else if (key == "esp_name_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.name_color_visible[i];
            }
        }
        else if (key == "esp_skeleton_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.skeleton_color_visible[i];
            }
        }
        else if (key == "esp_footstep_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.footstep_color_visible[i];
            }
        }
        else if (key == "esp_health_bar_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.health_bar_color_visible[i];
            }
        }
        else if (key == "esp_armor_bar_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.armor_bar_color_visible[i];
            }
        }
        else if (key == "esp_filled_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.filled_color_visible[i];
            }
        }
        else if (key == "esp_distance_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.distance_color_visible[i];
            }
        }
        else if (key == "esp_money_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.money_color_visible[i];
            }
        }
        else if (key == "esp_ping_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.ping_color_visible[i];
            }
        }
        else if (key == "esp_dropped_weapon_color_visible") {
            for (int i = 0; i < 4; ++i) {
                iss >> functions.esp.dropped_weapon_color_visible[i];
            }
        }
        
        else if (key == "rainbow_box") {
            int value; iss >> value; functions.esp.rainbow_box = (value != 0);
        }
        else if (key == "rainbow_filled") {
            int value; iss >> value; functions.esp.rainbow_filled = (value != 0);
        }
        else if (key == "rainbow_footstep") {
            int value; iss >> value; functions.esp.rainbow_footstep = (value != 0);
        }
        else if (key == "rainbow_skeleton") {
            int value; iss >> value; functions.esp.rainbow_skeleton = (value != 0);
        }
        else if (key == "rainbow_tracer") {
            int value; iss >> value; functions.esp.rainbow_tracer = (value != 0);
        }
        else if (key == "rainbow_offscreen") {
            int value; iss >> value; functions.esp.rainbow_offscreen = (value != 0);
        }
        else if (key == "rainbow_health_bar") {
            int value; iss >> value; functions.esp.rainbow_health_bar = (value != 0);
        }
        else if (key == "rainbow_armor_bar") {
            int value; iss >> value; functions.esp.rainbow_armor_bar = (value != 0);
        }
        else if (key == "rainbow_name") {
            int value; iss >> value; functions.esp.rainbow_name = (value != 0);
        }
        else if (key == "rainbow_distance") {
            int value; iss >> value; functions.esp.rainbow_distance = (value != 0);
        }
        else if (key == "rainbow_money") {
            int value; iss >> value; functions.esp.rainbow_money = (value != 0);
        }
        else if (key == "rainbow_ping") {
            int value; iss >> value; functions.esp.rainbow_ping = (value != 0);
        }
        else if (key == "rainbow_bomber") {
            int value; iss >> value; functions.esp.rainbow_bomber = (value != 0);
        }
        else if (key == "rainbow_defuser") {
            int value; iss >> value; functions.esp.rainbow_defuser = (value != 0);
        }
        else if (key == "rainbow_weapon") {
            int value; iss >> value; functions.esp.rainbow_weapon = (value != 0);
        }
        else if (key == "rainbow_weapon_text") {
            int value; iss >> value; functions.esp.rainbow_weapon_text = (value != 0);
        }
        else if (key == "rainbow_dropped_weapon") {
            int value; iss >> value; functions.esp.rainbow_dropped_weapon = (value != 0);
        }
        
        else if (key == "rainbow_box_visible") {
            int value; iss >> value; functions.esp.rainbow_box_visible = (value != 0);
        }
        else if (key == "rainbow_filled_visible") {
            int value; iss >> value; functions.esp.rainbow_filled_visible = (value != 0);
        }
        else if (key == "rainbow_footstep_visible") {
            int value; iss >> value; functions.esp.rainbow_footstep_visible = (value != 0);
        }
        else if (key == "rainbow_skeleton_visible") {
            int value; iss >> value; functions.esp.rainbow_skeleton_visible = (value != 0);
        }
        else if (key == "rainbow_tracer_visible") {
            int value; iss >> value; functions.esp.rainbow_tracer_visible = (value != 0);
        }
        else if (key == "rainbow_offscreen_visible") {
            int value; iss >> value; functions.esp.rainbow_offscreen_visible = (value != 0);
        }
        else if (key == "rainbow_health_bar_visible") {
            int value; iss >> value; functions.esp.rainbow_health_bar_visible = (value != 0);
        }
        else if (key == "rainbow_armor_bar_visible") {
            int value; iss >> value; functions.esp.rainbow_armor_bar_visible = (value != 0);
        }
        else if (key == "rainbow_name_visible") {
            int value; iss >> value; functions.esp.rainbow_name_visible = (value != 0);
        }
        else if (key == "rainbow_distance_visible") {
            int value; iss >> value; functions.esp.rainbow_distance_visible = (value != 0);
        }
        else if (key == "rainbow_money_visible") {
            int value; iss >> value; functions.esp.rainbow_money_visible = (value != 0);
        }
        else if (key == "rainbow_ping_visible") {
            int value; iss >> value; functions.esp.rainbow_ping_visible = (value != 0);
        }
        else if (key == "rainbow_bomber_visible") {
            int value; iss >> value; functions.esp.rainbow_bomber_visible = (value != 0);
        }
        else if (key == "rainbow_defuser_visible") {
            int value; iss >> value; functions.esp.rainbow_defuser_visible = (value != 0);
        }
        else if (key == "rainbow_weapon_visible") {
            int value; iss >> value; functions.esp.rainbow_weapon_visible = (value != 0);
        }
        else if (key == "rainbow_weapon_text_visible") {
            int value; iss >> value; functions.esp.rainbow_weapon_text_visible = (value != 0);
        }
        else if (key == "rainbow_dropped_weapon_visible") {
            int value; iss >> value; functions.esp.rainbow_dropped_weapon_visible = (value != 0);
        }
        else if (key == "rainbow_speed") {
            iss >> functions.esp.rainbow_speed;
        }
        
        
        
        else if (key == "esp_footstep_delta") {
            iss >> functions.esp.footstep_delta;
        }
        else if (key == "esp_footstep_speed") {
            iss >> functions.esp.footstep_speed;
        }
        
        else if (key == "esp_box_width") {
            iss >> functions.esp.box_width;
        }
        else if (key == "esp_box_outline_width") {
            iss >> functions.esp.box_outline_width;
        }
        else if (key == "esp_box_outline_enabled") {
            int value;
            iss >> value;
            functions.esp.box_outline_enabled = (value != 0);
        }
        else if (key == "esp_skeleton_width") {
            iss >> functions.esp.skeleton_width;
        }
        else if (key == "esp_tracer_width") {
            iss >> functions.esp.tracer_width;
        }
        else if (key == "esp_health_bar_width") {
            iss >> functions.esp.health_bar_width;
        }
        else if (key == "esp_health_bar_outline_enabled") {
            int value;
            iss >> value;
            functions.esp.health_bar_outline_enabled = (value != 0);
        }
        else if (key == "esp_armor_bar_width") {
            iss >> functions.esp.armor_bar_width;
        }
        else if (key == "esp_armor_bar_outline_enabled") {
            int value;
            iss >> value;
            functions.esp.armor_bar_outline_enabled = (value != 0);
        }
        else if (key == "esp_health_bar_offset") {
            iss >> functions.esp.health_bar_offset;
        }
        else if (key == "esp_health_bar_position_offset") {
            iss >> functions.esp.health_bar_position_offset;
        }
        else if (key == "esp_armor_bar_offset") {
            iss >> functions.esp.health_bar_offset;
        }
        else if (key == "esp_armor_bar_offset") {
            iss >> functions.esp.armor_bar_offset;
        }
        else if (key == "esp_name_offset") {
            iss >> functions.esp.name_offset;
        }
        else if (key == "esp_weapon_name_offset") {
            iss >> functions.esp.weapon_name_offset;
        }
        else if (key == "esp_weapon_icon_offset") {
            iss >> functions.esp.weapon_icon_offset;
        }
        else if (key == "esp_weapon_icon_size") {
            iss >> functions.esp.weapon_icon_size;
        }
        else if (key == "esp_health_bar_position") {
            iss >> functions.esp.health_bar_position;
        }
        else if (key == "esp_armor_bar_position") {
            iss >> functions.esp.armor_bar_position;
        }
        else if (key == "esp_name_position") {
            iss >> functions.esp.name_position;
        }
        else if (key == "esp_weapon_name_position") {
            iss >> functions.esp.weapon_name_position;
        }
        
        
        else if (key == "hitlog_enabled") {
            int value;
            iss >> value;
            hitLogger.enabled = (value != 0);
        }
        else if (key == "hitlog_show_headshot") {
            int value;
            iss >> value;
            hitLogger.showHeadshotIcon = (value != 0);
        }
        else if (key == "hitlog_show_kill") {
            int value;
            iss >> value;
            hitLogger.showKillIcon = (value != 0);
        }
        else if (key == "hitlog_show_weapon") {
            int value;
            iss >> value;
            hitLogger.showWeapon = (value != 0);
        }
        else if (key == "hitlog_show_damage") {
            int value;
            iss >> value;
            hitLogger.showDamage = (value != 0);
        }
        else if (key == "hitlog_display_time") {
            iss >> hitLogger.displayTime;
        }
        else if (key == "hitlog_fade_delay") {
            iss >> hitLogger.fadeDelay;
        }
        else if (key == "hitlog_max_entries") {
            iss >> hitLogger.maxEntries;
        }
        else if (key == "hitlog_position_x") {
            iss >> hitLogger.positionX;
        }
        else if (key == "hitlog_position_y") {
            iss >> hitLogger.positionY;
        }
        else if (key == "hitlog_text_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> hitLogger.textColor[i];
            }
        }
        else if (key == "hitlog_damage_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> hitLogger.damageColor[i];
            }
        }
        else if (key == "hitlog_headshot_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> hitLogger.headshotColor[i];
            }
        }
        else if (key == "hitlog_kill_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> hitLogger.killColor[i];
            }
        }
        else if (key == "hitlog_weapon_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> hitLogger.weaponColor[i];
            }
        }
        else if (key == "hitlog_bg_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> hitLogger.backgroundColor[i];
            }
        }
        
        else if (key == "tracer_enabled") {
            int value;
            iss >> value;
            bulletTracer.enabled = (value != 0);
        }
        else if (key == "tracer_display_time") {
            iss >> bulletTracer.displayTime;
        }
        else if (key == "tracer_fade_time") {
            iss >> bulletTracer.fadeTime;
        }
        else if (key == "tracer_line_thickness") {
            iss >> bulletTracer.lineThickness;
        }
        else if (key == "tracer_use_gradient") {
            int value;
            iss >> value;
            bulletTracer.useGradient = (value != 0);
        }
        else if (key == "tracer_color1") {
            for (int i = 0; i < 4; ++i) {
                iss >> bulletTracer.color1[i];
            }
        }
        else if (key == "tracer_color2") {
            for (int i = 0; i < 4; ++i) {
                iss >> bulletTracer.color2[i];
            }
        }
        else if (key == "tracer_stop_on_hit") {
            int value;
            iss >> value;
            bulletTracer.stopOnHit = (value != 0);
        }
        else if (key == "tracer_show_hit_point") {
            int value;
            iss >> value;
            bulletTracer.showHitPoint = (value != 0);
        }
        else if (key == "tracer_hit_point_radius") {
            iss >> bulletTracer.hitPointRadius;
        }
        else if (key == "tracer_hit_point_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> bulletTracer.hitPointColor[i];
            }
        }
        else if (key == "tracer_max_traces") {
            iss >> bulletTracer.maxTraces;
        }
        
        else if (key == "hitmarker_enabled") {
            int value;
            iss >> value;
            hitMarkerSettings.enabled = (value != 0);
        }
        else if (key == "hitmarker_type") {
            iss >> hitMarkerSettings.markerType;
        }
        else if (key == "hitmarker_display_time") {
            iss >> hitMarkerSettings.displayTime;
        }
        else if (key == "hitmarker_fade_time") {
            iss >> hitMarkerSettings.fadeTime;
        }
        else if (key == "hitmarker_use_hit_type_colors") {
            int value;
            iss >> value;
            hitMarkerSettings.useHitTypeColors = (value != 0);
        }
        else if (key == "hitmarker_hit_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> hitMarkerSettings.hitColor[i];
            }
        }
        else if (key == "hitmarker_headshot_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> hitMarkerSettings.headshotColor[i];
            }
        }
        else if (key == "hitmarker_kill_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> hitMarkerSettings.killColor[i];
            }
        }
        else if (key == "hitmarker_max_markers") {
            iss >> hitMarkerSettings.maxMarkers;
        }
        
        else if (key == "aim_enabled") {
            int value;
            iss >> value;
            aimSettings.enabled = (value != 0);
        }
        else if (key == "aim_draw_fov") {
            int value;
            iss >> value;
            aimSettings.drawFov = (value != 0);
        }
        else if (key == "aim_draw_fov_outline") {
            int value;
            iss >> value;
            aimSettings.drawFovOutline = (value != 0);
        }
        else if (key == "aim_fov_check") {
            int value;
            iss >> value;
            aimSettings.fovCheck = (value != 0);
        }
        else if (key == "aim_check_visible") {
            int value;
            iss >> value;
            aimSettings.checkVisible = (value != 0);
        }
        else if (key == "aim_triggerbot") {
            int value;
            iss >> value;
            aimSettings.triggerBot = (value != 0);
        }
        else if (key == "aim_fire_check") {
            int value;
            iss >> value;
            aimSettings.fireCheck = (value != 0);
        }
        else if (key == "aim_fov") {
            iss >> aimSettings.fov;
        }
        else if (key == "aim_smooth") {
            iss >> aimSettings.smooth;
        }
        else if (key == "aim_trigger_delay") {
            iss >> aimSettings.triggerDelay;
        }
        else if (key == "aim_max_distance") {
            iss >> aimSettings.maxDistance;
        }
        else if (key == "aim_target_switch_delay") {
            iss >> aimSettings.targetSwitchDelay;
        }
        else if (key == "aim_kill_switch_delay") {
            iss >> aimSettings.killSwitchDelay;
        }
        else if (key == "aim_selected_bones") {
            for (int i = 0; i < 14; i++) {
                int val = 0;
                iss >> val;
                aimSettings.selectedBones[i] = (val != 0);
            }
        }
        else if (key == "aim_selected_bone") {
            
            int bone;
            iss >> bone;
            for (int i = 0; i < 14; i++) aimSettings.selectedBones[i] = false;
            if (bone >= 0 && bone < 14) aimSettings.selectedBones[bone] = true;
        }
        else if (key == "aim_hit_chance") {
            iss >> aimSettings.hitChance;
            aimSettings.hitChance = std::clamp(aimSettings.hitChance, 0, 100);
        }
        else if (key == "aim_fov_color") {
            for (int i = 0; i < 4; ++i) {
                iss >> aimSettings.fovColor[i];
            }
        }
        
        else if (key == "recoil_enabled") {
            int value;
            iss >> value;
            recoilSettings.enabled = (value != 0);
        }
        else if (key == "recoil_horizontal") {
            iss >> recoilSettings.horizontalValue;
        }
        else if (key == "recoil_vertical") {
            iss >> recoilSettings.verticalValue;
        }
        else if (key == "recoil_no_progress") {
            
        }
        else if (key == "recoil_progress") {
            
        }
        else if (key == "recoil_value") {
            
            int val;
            iss >> val;
            recoilSettings.horizontalValue = val;
            recoilSettings.verticalValue = val;
        }
        else if (key == "recoil_no_spread") {
            int value;
            iss >> value;
            recoilSettings.noSpread = (value != 0);
        }

    }

    file.close();
    std::cout << "Config loaded successfully: " << filepath << std::endl;
    return true;
}

void ConfigManager::WriteFloatArray(std::ofstream& file, const std::string& name, float* values, int count) {
    file << name;
    for (int i = 0; i < count; ++i) {
        file << " " << values[i];
    }
    file << "\n";
}

void ConfigManager::WriteFloat(std::ofstream& file, const std::string& name, float value) {
    file << name << " " << value << "\n";
}

bool ConfigManager::CreateDirectoryIfNotExists(const std::string& path) {
    if (path.empty()) return true;
    
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        
        #ifdef _WIN32
            return _mkdir(path.c_str()) == 0;
        #else
            return mkdir(path.c_str(), 0755) == 0;
        #endif
    } else if (info.st_mode & S_IFDIR) {
        
        return true;
    }
    return false;
}

std::string ConfigManager::GetDirectoryFromPath(const std::string& filepath) {
    size_t found = filepath.find_last_of("/\\");
    if (found != std::string::npos) {
        return filepath.substr(0, found);
    }
    return "";
}
