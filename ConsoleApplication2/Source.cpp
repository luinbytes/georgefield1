#include "Includes.h"
#include <iostream>

bool valid(uintptr_t ptr, std::string text)
{
#ifndef NDEBUG
    if (ptr >= 0x10000 && ptr < 0x000F000000000000)
        return true;
    balls::log(std::format("{} was invalid", text));
    return false;
#else
    return (ptr >= 0x10000 && ptr < 0x000F000000000000);
#endif
}
#define is_valid(var) valid(var,#var)

void balls::init()
{
    console = GetStdHandle(STD_OUTPUT_HANDLE);
    log("initialiezing!!!!", gude);
    auto george = Memory(L"bf4.exe");

    log("dONE!", gude);
    std::thread(moveing, george).detach();
    std::thread(handle_menu).detach();
    while (!key_held(VK_END))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void move_mouse(vec3 target)
{
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE;
    Input.mi.dx = (target.x - balls::width / 2) / balls::smooth;
    Input.mi.dy = (target.y - balls::height / 2) / balls::smooth;
    SendInput(1, &Input, sizeof(INPUT));
}

bool get_bone(const Memory& mem, uintptr_t soldier, const int bone_id, vec3& out)
{
    const auto ragdoll = mem.Read<uintptr_t>(soldier + Offsets::ragdollTransforms);
    if (!is_valid(ragdoll))
        return false;

    const auto validtransform = mem.Read<unsigned char>(ragdoll + Offsets::validTransforms);
    if (validtransform == 0)
        return false;

    const auto pQuat = mem.Read<uintptr_t>(ragdoll + 0x0010);
    if (!is_valid(pQuat))
        return false;

    out = mem.Read<vec3>(pQuat + bone_id * 0x20);

    return true;
}

bool balls::world_to_screen(const Memory& mem, const vec3& WorldPos, vec3& ScreenPos)
{
    float cX = balls::width * 0.5f;
    float cY = balls::height * 0.5f;

    float w = balls::view_x_projection(0, 3) * WorldPos.x + balls::view_x_projection(1, 3) * WorldPos.y + balls::view_x_projection(2, 3) * WorldPos.z + balls::view_x_projection(3, 3);

    if (w < 0.0001f)
    {
        ScreenPos.z = w;
        return false;
    }

    float x = balls::view_x_projection(0, 0) * WorldPos.x + balls::view_x_projection(1, 0) * WorldPos.y + balls::view_x_projection(2, 0) * WorldPos.z + balls::view_x_projection(3, 0);
    float y = balls::view_x_projection(0, 1) * WorldPos.x + balls::view_x_projection(1, 1) * WorldPos.y + balls::view_x_projection(2, 1) * WorldPos.z + balls::view_x_projection(3, 1);

    ScreenPos.x = cX + cX * x / w;
    ScreenPos.y = cY - cY * y / w;
    ScreenPos.z = w;

    return true;
}
float dist_from_crosshair(vec3 spot)
{
    auto x = abs(balls::width / 2.0f - spot.x);
    auto y = abs(balls::height / 2.0f - spot.y);
    return x > y ? x : y;
}

vec3 chatbot_predict(vec3 localorigin, vec3 targetPosition, vec3 targetVelocity, vec3 localVelocity, float bulletVelocity, float bulletGravity)
{
    // Calculate the vector from the local origin to the target position
    vec3 startToEnd = targetPosition - localorigin;

    // Calculate the distance from the local origin to the target position
    float distance = startToEnd.length();

    // Calculate the time it takes for the bullet to reach the target
    float time = distance / bulletVelocity;

    // Calculate the gravitational acceleration at the location where the calculation is being performed

    // Update the target position to account for bullet drop
    targetPosition.y += 0.5 * bulletGravity * time * time;

    // Calculate the projected position of the target at the time the bullet reaches it
    vec3 predictedPosition = targetPosition + targetVelocity * time;

    // Adjust the predicted position to account for the local velocity of the shooter
    predictedPosition -= localVelocity * time;

    return predictedPosition;
}

uintptr_t read_and_validate_memory_address(const Memory& mem, const uintptr_t address, std::vector<uintptr_t> balls)
{
    if (balls.empty())
        return 0;
    auto adrs = address;
    for (auto ball : balls)
    {
        adrs = mem.Read<uintptr_t>(adrs + ball);
        if (!is_valid(adrs))
            return 0;
    }
    return adrs;
}

//bool in_veh(const Memory& mem, uintptr_t ent)
//{
//    const auto pAttached = mem.Read<uintptr_t>(ent + 0x14C0);
//    if (!pAttached)
//        return false;
//
//    for (int b = 0; b < 70; b++)
//    {
//        const auto s = mem.Read<uintptr_t>(0x1424794F0 + (b * 0x18));
//        const auto c = mem.Read<uintptr_t>(s + 0x1E0);
//        if (c == ent && s != 0)
//        {
//            return true;
//        }
//    }
//    return false;
//}

void balls::moveing(const Memory& mem)
{
    const auto gamecontext = mem.Read<uintptr_t>(Offsets::ClientGameContext);
    if (!is_valid(gamecontext))
    {
        balls::log("gamecontext was invalid", eror);
        return;
    }

    const auto dxrenderer = mem.Read<uintptr_t>(0x1439e6d08);
    if (!is_valid(dxrenderer))
    {
        balls::log("dxrenderer was invalid", eror);
        return;
    }

    const auto screensize = mem.Read<uintptr_t>(dxrenderer + 0x38);
    if (!is_valid(screensize))
    {
        balls::log("screensize was invalid", eror);
        return;
    }

    const auto player_mngr = mem.Read<uintptr_t>(gamecontext + Offsets::ClientPlayerManager);
    if (!is_valid(player_mngr))
    {
        balls::log("player_mngr was invalid", eror);
        return;
    }
    //balls::log(std::format("player_mngr: {:#x}", player_mngr), info);
    //const auto playerlist = mem.Read<uintptr_t>(player_mngr + offsets.player);
    //if (!is_valid(playerlist))
    //{
    //    balls::log("playerlist was invalid", eror);
    //    return;
    //}

    static auto game_render = mem.Read<uintptr_t>(Offsets::GameRender);
    if (!is_valid(game_render))
    {
        balls::log("game_render was invalid", eror);
        return;
    }


    static auto renderview = mem.Read<uintptr_t>(game_render + 0x0060);
    if (!is_valid(renderview))
    {
        balls::log("renderview was invalid", eror);
        return;
    }

    //while (true)
    //{
    //    width = mem.Read<int>(screensize + 0x58);
    //    height = mem.Read<int>(screensize + 0x5C);

    //    const auto localplayer = mem.Read<uintptr_t>(player_mngr + offsets.local_player);
    //    if (!is_valid(localplayer))
    //        continue;

    //    const auto localteam = mem.Read<int>(localplayer + offsets.team);

    //    const auto localsoldier = mem.Read<uintptr_t>(localplayer + offsets.soldier);
    //    if (!is_valid(localsoldier))
    //        continue;

    //    const auto lphptemp = mem.Read<uintptr_t>(localsoldier + offsets.healthclass);
    //    if (!is_valid(lphptemp))
    //        continue;

    //    const auto lphealth = mem.Read<float>(lphptemp + offsets.health);
    //    if (lphealth <= 0)
    //        continue;

    //    balls::view_x_projection = mem.Read<D3DXMATRIXA16>(renderview + 0x420);
    //    balls:: view_inverse = mem.Read<D3DXMATRIXA16>(renderview + 0x2E0);
    //    const auto local_pos = (vec3)&view_inverse._41;

    //    const auto pos_ptr = mem.Read<uintptr_t>(localsoldier + offsets.position);
    //    if (!is_valid(pos_ptr))
    //        continue;

    //    const auto local_velocity = mem.Read<vec3>(pos_ptr + offsets.position_velocity);

    //    const auto curwpn = mem.Read<uintptr_t>(0x1423B2EC8);
    //    if (!is_valid(curwpn))
    //        continue;

    //    auto firedata = read_and_validate_memory_address(mem, curwpn, { 0x128, 0x10 });
    //    if (!is_valid(firedata))
    //        continue;
    //    const auto bullet_speed = mem.Read<vec3>(firedata + 0x60 + 0x20).z;

    //    const auto projdata = mem.Read<uintptr_t>(firedata + 0x60 + 0x50);
    //    if (!is_valid(projdata))
    //        continue;
    //    const auto bullet_gravity = mem.Read<float>(projdata + 0x0130) * -1;

    //    vec3 bestheadpos(0, 0, 0);
    //    float closestfov = balls::fov;
    //    for (size_t i = 0; i < 70; i++)
    //    {
    //        const auto ent = mem.Read<uintptr_t>(playerlist + (i * 0x08));
    //        if (!is_valid(ent) || ent == localplayer)
    //            continue;

    //        const auto soldier = mem.Read<uintptr_t>(ent + offsets.soldier);
    //        if (!is_valid(soldier))
    //            continue;

    //        const auto hptemp = mem.Read<uintptr_t>(soldier + offsets.healthclass);
    //        if (!is_valid(hptemp))
    //            continue;

    //        const auto health = mem.Read<float>(hptemp + offsets.health);
    //        if (health <= 0)
    //            continue;

    //        const auto occluded = mem.Read<bool>(soldier + 0x5B1);
    //        if (occluded /*&& !in_veh(mem, ent)*/)
    //            continue;

    //        const auto team = mem.Read<int>(ent + offsets.team);
    //        if (localteam == team)
    //            continue;

    //        const auto pos = mem.Read<uintptr_t>(soldier + offsets.position);
    //        if (!is_valid(pos))
    //            continue;

    //        const auto velocity = mem.Read<vec3>(pos + offsets.position_velocity);
    //        //mem.Write<BYTE>(soldier + 0x1A, 159); // updates bone visibility something?

    //        std::vector<int> bones;

    //        switch (balls::aimpriority)
    //        {
    //        case 1:
    //            bones = { m_HEAD };
    //            break;
    //        case 2:
    //            bones = { m_HIPS, m_PELVIS, m_UPPERCHEST };
    //            break;
    //        default:
    //            bones = { m_HEAD, m_HIPS, m_PELVIS, m_UPPERCHEST /*m_LEFTLEG, m_RIGHTLEG, m_LEFTFOOT, m_RIGHTFOOT*/ };
    //            break;
    //        }

    //        for (int g = 0; g < bones.size(); g++)
    //        {
    //            vec3 bone(0, 0, 0);
    //            if (!get_bone(mem, soldier, bones[g], bone))
    //                continue;
    //            bone = chatbot_predict(local_pos, bone, velocity, local_velocity, bullet_speed, bullet_gravity);
    //            vec3 balls;
    //            auto w2s = balls::world_to_screen(mem, bone, balls);
    //            if (!w2s || balls.x < 0 || balls.x > balls::width || balls.y < 0 || balls.y > balls::height)
    //                continue;

    //            auto dist = dist_from_crosshair(balls);
    //            if (dist < closestfov)
    //            {
    //                closestfov = dist;
    //                bestheadpos = balls;
    //            }
    //        }
    //    }

    //    if (bestheadpos.x > 0 && bestheadpos.y > 0 && (balls::key_held(VK_RBUTTON) && balls::key_held(VK_LBUTTON) || balls::key_held(VK_XBUTTON2)))
    //        move_mouse(bestheadpos);

    //    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //}
}

void balls::log(std::string tekst, log_ yes)
{
    SetConsoleTextAttribute(console, yes);
    std::cout << "[+]" << tekst << '\n';
}

void balls::handle_menu()
{
    while (true)
    {
        if ((GetAsyncKeyState(VK_UP) & 1))
            balls::smooth += 0.1;
        if ((GetAsyncKeyState(VK_DOWN) & 1))
            balls::smooth -= 0.1;
        if ((GetAsyncKeyState(VK_RIGHT) & 1))
            balls::fov += 2;
        if ((GetAsyncKeyState(VK_LEFT) & 1))
            balls::fov -= 2;

        if ((GetAsyncKeyState(VK_RSHIFT) & 1))
            balls::aimpriority++;

        if (balls::aimpriority > 2)
            balls::aimpriority = 0;

        static bool balls = false;
        balls = !balls;
        if (balls)
            std::rotate(std::begin(grogtext), std::next(std::begin(grogtext)), std::end(grogtext));

        balls::fov = std::clamp(balls::fov, 1.f, 300.f);
        balls::smooth = std::clamp(balls::smooth, 1.f, 20.f);
        const std::string aimtypes[3] = { "all","head","body" };
        SetConsoleTitleA(std::format("{} fov {:.1f}, smoothing {:.1f}, hb: {}  :)", grogtext, balls::fov, balls::smooth, aimtypes[balls::aimpriority]).data());

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}