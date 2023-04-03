#include "Includes.h"
#include <Windows.h>

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

class QuatTransform
{
public:
    D3DXVECTOR4 m_TransAndScale; //0x0000 
    D3DXVECTOR4 m_Rotation; //0x0010 
};//Size=0x0020

bool get_bone(const Memory& mem, uintptr_t soldier, const int bone_id, vec3& out)
{
    const auto ragdoll = mem.Read<uintptr_t>(soldier + Offsets::bonecollisioncomponent);
    if (!is_valid2(ragdoll))
        return false;

    const auto validtransform = mem.Read<unsigned char>(ragdoll + 0x0038);
    if (validtransform == 0)
        return false;

    const auto pQuat = mem.Read<uintptr_t>(ragdoll + 0x0028);
    if (!is_valid2(pQuat))
        return false;

    out = mem.Read<vec3>(pQuat + bone_id * 0x20);

    return true;
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

float dist_from_crosshair(vec3 spot)
{
    auto x = abs(1920 / 2.0f - spot.x);
    auto y = abs(1080 / 2.0f - spot.y);
    return x > y ? x : y;
}

bool world_to_screen(const Memory& mem, const vec3& WorldPos, vec3& ScreenPos)
{
    float cX = 1920 * 0.5f;
    float cY = 1080 * 0.5f;
    auto view_x_projection = mem.Read<D3DXMATRIXA16>(Offsets::viewProj);
    

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

        //const auto playerlist = mem.Read<uintptr_t>(Offsets::clientSoldierEntity + 0x268);
        //if (!is_valid2(playerlist))
        //{
        //    b::log("playerlist was invalid", eror);
        //}

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

        b::view_x_projection = mem.Read<D3DXMATRIXA16>(renderview + 0x0460);

		/*uintptr_t LocalPlayer = GetLocalPlayer(mem);
        if (!is_valid2(LocalPlayer))
        {
			b::log("LocalPlayer was invalid", eror);
		}*/

        vec3 bestheadpos(0, 0, 0);
        float closestfov = b::fov;
        uint64_t LocalPlayer = GetLocalPlayer(mem);
        if (!is_valid2(LocalPlayer))
        {
			b::log("LocalPlayer was invalid", eror);
		}

        auto LocalPlayerTeem = mem.Read<int>(LocalPlayer + Offsets::teamId);
        //b::log(std::format("local : {}", LocalPlayerTeem));

		//https://github.com/Zakaria-Master/BF1-ESP-AND-AIMBOT/blob/main/PZ-HAX/Frosbite.h#L473-L475
		for (size_t i = 0; i < 100; i++)
		{

			uint64_t Players = GetPlayerById(mem, i);

            uint64_t clientSoldierEntity = mem.Read<uint64_t>(Players + Offsets::clientSoldierEntity);
            uint64_t HealthComponent = mem.Read<uint64_t>(clientSoldierEntity + 0x01D0);

            const auto health = mem.Read<float>(HealthComponent + 0x0020);
            if (health < 1)
				continue;
            
            const auto occluded = mem.Read<bool>(clientSoldierEntity + Offsets::occluded);
            if (occluded)
                continue;

            auto PlayerTeem = mem.Read<int>(Players + Offsets::teamId);
            b::log(std::format("player:    {}", PlayerTeem));

            if (PlayerTeem != LocalPlayerTeem)
				continue;

            vec3 bone(0, 0, 0);
            if (!get_bone(mem, clientSoldierEntity, 0x35, bone))
                continue;

            vec3 balls;
            auto w2s = world_to_screen(mem, bone, balls);
            if (!w2s || balls.x < 0 || balls.x > 1920 || balls.y < 0 || balls.y > 1080)
                continue;

            auto dist = dist_from_crosshair(balls);
            //b::log(std::format("dist from xhair:   {}  {}", balls.x, balls.y), eror);
            if (dist < closestfov)
            {
                closestfov = dist;
                bestheadpos = balls;
            }

		}
        if (bestheadpos.x > 0 && bestheadpos.y > 0 && (key_held(VK_RBUTTON) && key_held(VK_LBUTTON) || key_held(VK_XBUTTON2) || key_held(VK_LBUTTON))) {
            move_mouse(bestheadpos);
            b::log(std::to_string(bestheadpos.x), eror);
            b::log(std::to_string(bestheadpos.y), eror);
            b::log("-----------------------------------", eror);
        }

		Sleep(1);
	}

	//return 0;
}
