#include "Includes.h"
#include <iostream>

int main(const Memory& mem)
{
	uintptr_t LocalPlayer = GetLocalPlayer();
	uintptr_t ClientPlayer  =  mem.Read<uintptr_t>(LocalPlayer + 0x0018);
	std::cout << mem.Read(ClientPlayer, 24);

	for (size_t i = 0; i < 100; i++)
	{
		uint64_t Players = GetPlayerById(i);

		uint64_t clientSoldierEntity = mem.Read<uint64_t>(Players + 0x1D48);
		uint64_t HealthComponent = mem.Read<uint64_t>(clientSoldierEntity + 0x1D0);

		std::cout << mem.Read<float>(HealthComponent + 0x0020) << std::endl;

	}


	getchar();
	return 0;
}
