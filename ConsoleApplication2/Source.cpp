#include "Includes.h"
#include <iostream>
bool key_held(int key)
{
	return (GetAsyncKeyState(key) & 0x8000);
}

int main()
{
	auto mem = Memory(L"bf1.exe");
	do
	{
		mem = Memory(L"bf1.exe");
		Sleep(500);
		std::cout << "bf1 not found\n";
	} while (!mem.is_valid());

	while (!key_held(VK_END))
	{
		uintptr_t LocalPlayer = GetLocalPlayer(mem);

		//uintptr_t ClientPlayer = mem.Read<uintptr_t>(LocalPlayer + 0x0018);
		//std::cout << mem.Read<char*>(ClientPlayer,24 or 8 idk);
		//https://github.com/Zakaria-Master/BF1-ESP-AND-AIMBOT/blob/main/PZ-HAX/Frosbite.h#L473-L475
		for (size_t i = 0; i < 100; i++)
		{
			uint64_t Players = GetPlayerById(mem, i);

			uint64_t clientSoldierEntity = mem.Read<uint64_t>(Players + 0x1D48);
			uint64_t HealthComponent = mem.Read<uint64_t>(clientSoldierEntity + 0x1D0);

			//bonecollisioncomponent = clientSoldierEntity + 0x0490 
			//if bonecollisioncomponent + 0x0038 == 1 then return
			//ourQuatTransform = bonecollisioncomponent + 0x20 ??
			//bonepos = mem.Read<vec3>(ourQuatTransform + bone_id * 0x20????);
			//https://github.com/Zakaria-Master/BF1-ESP-AND-AIMBOT/blob/main/PZ-HAX/Frosbite.h#L305

			std::cout << mem.Read<float>(HealthComponent + 0x0020) << std::endl;	
		}
		Sleep(1);
	}

	return 0;
}
