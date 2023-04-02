#include "Includes.h"
bool key_held(int key)
{
	return (GetAsyncKeyState(key) & 0x8000);
}

bool valid(uintptr_t ptr, std::string text)
{
#ifndef NDEBUG
	if (ptr >= 0x10000 && ptr < 0x000F000000000000)
		return true;
	b::log(std::format("{} was invalid", text));
	return false;
#else
	return (ptr >= 0x10000 && ptr < 0x000F000000000000);
#endif
}
#define is_valid2(var) valid(var,#var)

void b::log(std::string tekst, log_ yes)
{
	SetConsoleTextAttribute(console, yes);
	std::cout << "[+]" << tekst << '\n';
}

void move_mouse(vec3 target)
{
	INPUT Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE;
	Input.mi.dx = (target.x - 1920 / 2) / b::smooth;
	Input.mi.dy = (target.y - 1080 / 2) / b::smooth;
	SendInput(1, &Input, sizeof(INPUT));
}

bool b::world_to_screen(const Memory& mem, const vec3& WorldPos, vec3& ScreenPos)
{
    float cX = 1920 * 0.5f;
    float cY = 1080 * 0.5f;

    float w = b::view_x_projection(0, 3) * WorldPos.x + b::view_x_projection(1, 3) * WorldPos.y + b::view_x_projection(2, 3) * WorldPos.z + b::view_x_projection(3, 3);

    if (w < 0.0001f)
    {
        ScreenPos.z = w;
        return false;
    }

    float x = b::view_x_projection(0, 0) * WorldPos.x + b::view_x_projection(1, 0) * WorldPos.y + b::view_x_projection(2, 0) * WorldPos.z + b::view_x_projection(3, 0);
    float y = b::view_x_projection(0, 1) * WorldPos.x + b::view_x_projection(1, 1) * WorldPos.y + b::view_x_projection(2, 1) * WorldPos.z + b::view_x_projection(3, 1);

    ScreenPos.x = cX + cX * x / w;
    ScreenPos.y = cY - cY * y / w;
    ScreenPos.z = w;

    return true;
}

int main()
{
	auto mem = Memory(L"bf1.exe");
	std::cout << "Initializing george!" << '\n';

	if (mem.is_valid())
	{
		std::cout << "found bf1.exe" << '\n';
	}
	else
	{
		std::cout << "could not find bf1.exe" << '\n';
	}

	while (!key_held(VK_END))
	{
        const auto gamecontext = mem.Read<uintptr_t>(Offsets::ClientGameContext);
        if (!is_valid2(gamecontext))
        {
            b::log("gamecontext was invalid", eror);
        }
        
        /*const auto level = mem.Read<uintptr_t>(gamecontext + offsets.level);
        if (!is_valid2(level))
        {
            b::log("level was invalid", eror);
            return;
        }*/

        /*const auto gameworld = mem.Read<uintptr_t>(level + offsets.gameworld);
        if (!is_valid(gameworld))
        {
            b::log("gameworld was invalid", eror);
            return;
        }*/

        /*const auto dxrenderer = mem.Read<uintptr_t>(b::offsets.dx);
        if (!is_valid(dxrenderer))
        {
            b::log("dxrenderer was invalid", eror);
            return;
        }*/

        /*const auto screensize = mem.Read<uintptr_t>(dxrenderer + 0x38);
        if (!is_valid(screensize))
        {
            b::log("screensize was invalid", eror);
            return;
        }*/

        const auto player_mngr = mem.Read<uintptr_t>(gamecontext + 0x0068);
        if (!is_valid2(player_mngr))
        {
            b::log("player_mngr was invalid", eror);
        }

        /*const auto playerlist = mem.Read<uintptr_t>(player_mngr + offsets.player);
        if (!is_valid(playerlist))
        {
            b::log("playerlist was invalid", eror);
            return;
        }*/

        static auto game_render = mem.Read<uintptr_t>(Offsets::GameRender);
        if (!is_valid2(game_render))
        {
            b::log("game_render was invalid", eror);
        }


        static auto renderview = mem.Read<uintptr_t>(game_render + 0x0060);
        if (!is_valid2(renderview))
        {
            b::log("renderview was invalid", eror);
        }

        const auto LocalTeam = mem.Read<uintptr_t>(Offsets::teamId);

		uintptr_t LocalPlayer = GetLocalPlayer(mem);
		uintptr_t ClientPlayer = mem.Read<uintptr_t>(LocalPlayer + 0x0018);
        vec3 bestheadpos(0, 0, 0);
        float closestfov = b::fov;

		//https://github.com/Zakaria-Master/BF1-ESP-AND-AIMBOT/blob/main/PZ-HAX/Frosbite.h#L473-L475
		for (size_t i = 0; i < 100; i++)
		{
			uint64_t Players = GetPlayerById(mem, i);

			uint64_t clientSoldierEntity = mem.Read<uint64_t>(Players + 0x1D48);
			uint64_t HealthComponent = mem.Read<uint64_t>(clientSoldierEntity + 0x1D0);
            
            const auto occluded = mem.Read<uintptr_t>(clientSoldierEntity + Offsets::occluded);
            if (occluded)
                continue;

            std::vector<int> bones;

            switch (b::aimprority)
            {
            case 1:
                bones = { BONE_Head };
                break;
            case 2:
                bones = { BONE_Head, BONE_Spine, BONE_SPINE1 };
                break;
            case 3:
                bones = { BONE_Head, BONE_Spine, BONE_SPINE1, BONE_SPINE2 };
                break;
            }
		}
		Sleep(1);
	}

	//return 0;
}
