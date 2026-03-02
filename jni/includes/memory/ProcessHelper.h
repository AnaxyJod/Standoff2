

#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>

inline int GetPID(const char* name) {
    DIR *dir = opendir("/proc");
    if (!dir) return -1;
    
    struct dirent *entry;
    char path[64], cmdline[64];
    
    while ((entry = readdir(dir))) {
        int id = atoi(entry->d_name);
        if (id > 0) {
            snprintf(path, sizeof(path), "/proc/%d/cmdline", id);
            FILE *fp = fopen(path, "r");
            if (fp) {
                fgets(cmdline, sizeof(cmdline), fp);
                fclose(fp);
                if (strcmp(name, cmdline) == 0) {
                    closedir(dir);
                    return id;
                }
            }
        }
    }
    closedir(dir);
    return -1;
}

inline uintptr_t GetModuleBase(int pid, const char* name) {
    char path[64], line[1024];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE *fp = fopen(path, "r");
    if (!fp) return 0;
    
    uintptr_t base = 0;
    uintptr_t fallback = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, name)) {
            uintptr_t start, end;
            sscanf(line, "%lx-%lx", &start, &end);
            uintptr_t size = end - start;
            
            if (fallback == 0) fallback = start;
            
            if (strstr(line, "r--p") && size < 0x5100000) {
                base = start;
                break;
            }
        }
    }
    fclose(fp);
    
    return base ? base : fallback;
}
