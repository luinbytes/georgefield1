#pragma once
#define ValidPointer( pointer ) ( pointer != NULL && (DWORD_PTR)pointer >= 0x10000 && (DWORD_PTR)pointer < 0x00007FFFFFFEFFFF /*&& some other checks*/ )

uint64_t GetPlayerById(const Memory& mem, int id);
uint64_t GetLocalPlayer(const Memory& mem);
uint64_t GetSoldierWeapon(const Memory& mem);