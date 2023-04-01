#include "Includes.h"
#include <iostream>

int main()
{
	auto mem = Memory(L"bf1.exe");
	do
	{
		mem = Memory(L"bf1.exe");
		Sleep(500);
		std::cout << "bf1 not found\n";
	} while (!mem.is_valid());

	uintptr_t LocalPlayer = GetLocalPlayer(mem);
	uintptr_t ClientPlayer  =  mem.Read<uintptr_t>(LocalPlayer + 0x0018);
	//std::cout << mem.Read<char*>(ClientPlayer, 24);

	for (size_t i = 0; i < 100; i++)
	{
		uint64_t Players = GetPlayerById(mem, i);

		uint64_t clientSoldierEntity = mem.Read<uint64_t>(Players + 0x1D48);
		uint64_t HealthComponent = mem.Read<uint64_t>(clientSoldierEntity + 0x1D0);

		std::cout << mem.Read<float>(HealthComponent + 0x0020) << std::endl;
	}


	getchar();
	return 0;
}
