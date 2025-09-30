#pragma once
#ifndef MEMORY_READER_H
#define MEMORY_READER_H

#include <windows.h>
#include <vector>

struct Vector3 {
    float x, y, z;
};

struct Vector2 {
    float x, y;
};

struct PlayerEntity {
    Vector3 position;
    Vector3 headPosition;
    bool isAlive;
    float health;
    char name[32];
    int team;
};

namespace MemoryReader {
    bool Initialize(DWORD processId);
    void Shutdown();
    
    // Memory reading functions
    bool ReadVirtualMemory(ULONG_PTR address, void* buffer, SIZE_T size);
    bool WriteVirtualMemory(ULONG_PTR address, void* buffer, SIZE_T size);
    
    // Game-specific functions
    bool ReadViewMatrix(float* matrix);
    std::vector<PlayerEntity> ReadPlayerEntities();
    ULONG_PTR GetGameAssemblyBase();
    ULONG_PTR GetUnityPlayerBase();
    
    // Utility functions
    ULONG_PTR GetProcessCr3();
    bool IsValidAddress(ULONG_PTR address);

} // namespace MemoryReader

#endif // MEMORY_READER_H