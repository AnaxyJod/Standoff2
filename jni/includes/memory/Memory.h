

#pragma once
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <errno.h>
#include <cmath>

class MemoryHelper {
private:
    
    static ssize_t vm_readv(pid_t pid, const struct iovec *local, unsigned long liovcnt,
                           const struct iovec *remote, unsigned long riovcnt, unsigned long flags) {
        return syscall(SYS_process_vm_readv, pid, local, liovcnt, remote, riovcnt, flags);
    }
    
    static ssize_t vm_writev(pid_t pid, const struct iovec *local, unsigned long liovcnt,
                            const struct iovec *remote, unsigned long riovcnt, unsigned long flags) {
        return syscall(SYS_process_vm_writev, pid, local, liovcnt, remote, riovcnt, flags);
    }
    
    
    static bool isValidAddress(uint64_t addr) {
        
        
        if (addr < 0x10000) return false;  
        if (addr > 0x7fffffffffff) return false;  
        return true;
    }
    
public:
    int pid = 0;

    MemoryHelper() : pid(0) {}
    
    template<typename T>
    MemoryHelper(T&) : pid(0) {}
    
    ~MemoryHelper() {}

    void setPid(int newPid) {
        pid = newPid;
    }

    template <typename T>
    T read(uint64_t addr) {
        T data{};
        if (pid <= 0 || !isValidAddress(addr)) return data;
        
        struct iovec local = { &data, sizeof(T) };
        struct iovec remote = { (void*)addr, sizeof(T) };
        
        vm_readv(pid, &local, 1, &remote, 1, 0);
        return data;
    }

    template <typename T>
    bool write(uint64_t addr, T value) {
        if (pid <= 0 || !isValidAddress(addr)) return false;
        
        struct iovec local = { &value, sizeof(T) };
        struct iovec remote = { (void*)addr, sizeof(T) };
        
        return vm_writev(pid, &local, 1, &remote, 1, 0) == sizeof(T);
    }
    
    
    bool writeSafeFloat(uint64_t addr, float value) {
        if (std::isnan(value) || std::isinf(value)) return false;
        return write<float>(addr, value);
    }

    void readBuffer(uint64_t addr, void* buffer, size_t size) {
        if (pid <= 0 || !isValidAddress(addr) || buffer == nullptr || size == 0) return;
        
        struct iovec local = { buffer, size };
        struct iovec remote = { (void*)addr, size };
        
        vm_readv(pid, &local, 1, &remote, 1, 0);
    }

    void writeBuffer(uint64_t addr, void* buffer, size_t size) {
        if (pid <= 0 || !isValidAddress(addr) || buffer == nullptr || size == 0) return;
        
        struct iovec local = { buffer, size };
        struct iovec remote = { (void*)addr, size };
        
        vm_writev(pid, &local, 1, &remote, 1, 0);
    }
};
