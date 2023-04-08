#include "cock.h"

void b::init() {
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	log("Initializing georgefield1!", info);

	auto george = Memory(process_name);
    bool once = false;
 
    while (!george.Valid())
    { 
        if (!once)
        {
            log("Could not find process, retrying...", eror);
            once = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        george = Memory(process_name);
    }

	std::thread(aimbob,george).detach();
	std::thread(handle_menu).detach();
    log("george now owns your battlefield exe!!", gude);

	while (!key_held(VK_END))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

//Funny std::clamp doesnt work so all the homies now use clomp.
template<typename T>
constexpr const T& clomp(const T& value, const T& low, const T& high) {
    return value < low ? low : (high < value ? high : value);
}

void move_mouse(vec3 target) {
    auto x = (target.x - b::cw) / b::smooth;
    auto y = (target.y - b::ch) / b::smooth;

    if (abs(x) < 1.f && x)
        x = (x > 0) ? 1 : -1;

    if (abs(y) < 1.f && y)
        y = (y > 0) ? 1 : -1;

    mouse_event(MOUSEEVENTF_MOVE, (DWORD)(x), (DWORD)(y), 0, NULL);
}

void click() {
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &Input, sizeof(INPUT));
    Sleep(10);
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &Input, sizeof(INPUT));
}

bool get_bone(const Memory& mem, uintptr_t soldier, const int bone_id, vec3& out) {
    const auto ragdoll = mem.Read<uintptr_t>(soldier + 0x490);
    if (!is_valid(ragdoll))
        return false;

    const auto validtransform = mem.Read<bool>(ragdoll + 0x38);
    if (!validtransform)
        return false;

    const auto pQuat = mem.Read<uintptr_t>(ragdoll + 0x28);
    if (!is_valid(pQuat))
        return false;

    out = mem.Read<vec3>(pQuat + bone_id * 0x20);
   
	return true;
}

bool b::world_to_screen(const Memory& mem, const vec3& WorldPos, vec3& ScreenPos) {
    float w = view_x_projection(0, 3) * WorldPos.x + view_x_projection(1, 3) * WorldPos.y + view_x_projection(2, 3) * WorldPos.z + view_x_projection(3, 3);

    if (w < 0.0001f)
    {
        ScreenPos.z = w;
        return false;
    }

    float x = view_x_projection(0, 0) * WorldPos.x + view_x_projection(1, 0) * WorldPos.y + view_x_projection(2, 0) * WorldPos.z + view_x_projection(3, 0);
    float y = view_x_projection(0, 1) * WorldPos.x + view_x_projection(1, 1) * WorldPos.y + view_x_projection(2, 1) * WorldPos.z + view_x_projection(3, 1);

    ScreenPos.x = b::cw + b::cw * x / w;
    ScreenPos.y = b::ch - b::ch * y / w;
    ScreenPos.z = w;

    return true;
}

float dist_from_crosshair(vec3 spot) {
    auto x = fabs(b::cw - spot.x);
    auto y = fabs(b::ch - spot.y);
    return x > y ? x : y;
}

vec3 chatbot_predict(vec3 localorigin, vec3 targetPosition, vec3 targetVelocity, vec3 localVelocity, float bulletVelocity, float bulletGravity) {
    // Calculate the vector from the local origin to the target position
    vec3 startToEnd = targetPosition - localorigin;

    // Calculate the distance from the local origin to the target position
    float distance = startToEnd.length();

    // Calculate the time it takes for the bullet to reach the target
    float time = distance / bulletVelocity;

    // Calculate the gravitational acceleration at the location where the calculation is being performed

    // Update the target position to account for bullet drop
    targetPosition.y += 0.5f * bulletGravity * time * time;

    // Calculate the projected position of the target at the time the bullet reaches it
    vec3 predictedPosition = targetPosition + targetVelocity * time;

    // Adjust the predicted position to account for the local velocity of the shooter
    predictedPosition -= localVelocity * time;

    return predictedPosition;
}

uintptr_t read_and_validate_memory_address(const Memory& mem,const uintptr_t address, std::vector<uintptr_t> balls) {
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

void b::aimbob(const Memory& mem) {
    log("aimbob thread started!", gude);
    const auto gamecontext = mem.Read<uintptr_t>(offsets.game_context);
    if (!is_valid(gamecontext))
        return;

    const auto level = mem.Read<uintptr_t>(gamecontext + offsets.level);
    if (!is_valid(level))
        return;

    const auto gameworld = mem.Read<uintptr_t>(level + offsets.gameworld);
    if (!is_valid(gameworld))
        return;

    const auto player_mngr = mem.Read<uintptr_t>(gamecontext + offsets.player_manager);
    if (!is_valid(player_mngr))
        return;
    
    static auto game_render = mem.Read<uintptr_t>(offsets.game_render);
    if (!is_valid(game_render))
        return;

    static auto renderview = mem.Read<uintptr_t>(game_render + 0x60);
    if (!is_valid(renderview))
        return;

    //Hard code or no code.
    width = 1920;
    height = 1080;

    cw = width / 2;
    ch = height / 2;

	while (true)
	{
		const auto localplayer = GetLocalPlayer(mem);
		if (!is_valid(localplayer)) 
			continue;	

        const auto localteam = mem.Read<int>(localplayer + offsets.team);   

		const auto localsoldier = mem.Read<uintptr_t>(localplayer + offsets.soldier);
		if (!is_valid(localsoldier))
			continue;

		const auto lphptemp = mem.Read<uintptr_t>(localsoldier + offsets.healthclass);
		if (!is_valid(lphptemp))
			continue;

		const auto lphealth = mem.Read<float>(lphptemp + offsets.health);
		if (lphealth <= 0)
			continue;

        view_x_projection = mem.Read<D3DXMATRIXA16>(renderview + 0x0460);

       /*
       * 
       * Needs reversing smh.
       * 
        const auto pos_ptr = mem.Read<uintptr_t>(localsoldier + offsets.position);
        if (!is_valid(pos_ptr))
            continue;

        const auto local_velocity = mem.Read<vec3>(pos_ptr + offsets.position_velocity);   

        cur_wpn = mem.Read<uintptr_t>(offsets.cur_wpn);
        if (!is_valid(cur_wpn))
            continue;

        auto firedata = read_and_validate_memory_address(mem, cur_wpn, { 0x128, 0x10 });
        if (!is_valid(firedata))
            continue;

        const auto bullet_speed = mem.Read<vec3>(firedata + 0x60 + 0x20).z;

        const auto projdata = mem.Read<uintptr_t>(firedata + 0x60 + 0x50);
        if (!is_valid(projdata))
            continue;

        const auto bullet_gravity = mem.Read<float>(projdata + 0x0130) * -1;*/

        vec3 bestheadpos(0,0,0);
        vec3 pos3d;
        float closestfov = b::fov;
		for (size_t i = 0; i < 100; i++)
		{         
            const auto ent = GetPlayerById(mem, i);
			if (!is_valid(ent))
				continue;

            const auto soldier = mem.Read<uintptr_t>(ent + offsets.soldier);
            if (!is_valid(soldier))
                continue;

            const auto hptemp = mem.Read<uintptr_t>(soldier + offsets.healthclass);
            if (!is_valid(hptemp))
                continue;

            const auto health = mem.Read<float>(hptemp + offsets.health);
            if (health <= 0)
                continue;
          /*  
          * 
          * Probably not needed but I shall keep anyways.
          * 
            mem.Write<BYTE>(soldier + 0x1A, 159); // updates bone visibility something?
            const auto occluded = mem.Read<bool>(soldier + 0x6EB);
            if (occluded)
                continue;
            const auto team = mem.Read<int>(ent + offsets.team);
            log(std::format("{} enemy: {} local:{}", i, team, localteam));
            if (localteam == team)
                continue;
            const auto pos = mem.Read<uintptr_t>(soldier + offsets.position);
            if (!is_valid(pos))
                continue;
            
            const auto velocity = mem.Read<vec3>(pos + offsets.position_velocity);*/
            
            std::vector<int> bones; 

            switch (b::aimprority)
            {
            case 1:
                bones = { BONE_Head };
                break;
            case 2:
                bones = { BONE_SPINE2, BONE_Spine, BONE_SPINE1 };
                break;
            default:
                bones = { BONE_Head, BONE_SPINE2, BONE_Spine, BONE_SPINE1 /*m_LEFTLEG, m_RIGHTLEG, m_LEFTFOOT, m_RIGHTFOOT*/ };
                break;
            } 

            const auto ragdoll = mem.Read<uintptr_t>(soldier + 0x490);
            if (!is_valid(ragdoll))
                continue;

            const auto validtransform = mem.Read<bool>(ragdoll + 0x38);
            if (!validtransform)
                continue;

            const auto pQuat = mem.Read<uintptr_t>(ragdoll + 0x28);
            if (!is_valid(pQuat))
                continue;

            for (int g = 0; g < bones.size(); g++)
            {  
                vec3 bone = mem.Read<vec3>(pQuat + bones[g] * 0x20);

                /*
                * 
                * No velocity, will reverse once im smart.
                * 
                bone = chatbot_predict(local_pos, bone, velocity, local_velocity, bullet_speed, bullet_gravity);*/
                
                vec3 balls;
                auto w2s = world_to_screen(mem, bone, balls);

                if (!w2s || balls.x < 0 || balls.x > b::width || balls.y < 0 || balls.y > b::height)
                    continue;
                
                auto dist = dist_from_crosshair(balls);
                if (dist < closestfov)
                {
                    closestfov = dist;
                    bestheadpos = balls;
                    pos3d = bone;
                }
            }
		}

        if (bestheadpos.x > 0 && bestheadpos.y > 0 && (key_held(VK_RBUTTON) && key_held(VK_LBUTTON) || key_held(VK_XBUTTON2))) 
            move_mouse(bestheadpos);

        if(bestheadpos.x > 0 && bestheadpos.y > 0 && key_held(VK_XBUTTON1))
        {
            vec3 below;
            auto success = world_to_screen(mem, pos3d - vec3(0, 0.21, 0), below);

            float radius = (bestheadpos - below).length() / 2.0f;
            vec3 center_circle = (bestheadpos + below) / 2.0f;

            float distance = vec3(center_circle.x - cw, center_circle.y - ch, 0).length() / 2.f;
            if (distance <= radius) {
                click();
            }
        }

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void b::log(std::string tekst, log_ yes)
{
    SetConsoleTextAttribute(console, yes);
    std::cout << "[+] " << tekst << '\n';
}

void b::handle_menu() {   
    log("console thread started!", gude);
    while (true)
    {
        static auto console_window = GetConsoleWindow();
        if (GetForegroundWindow() == console_window)
        {
            if ((GetAsyncKeyState(VK_UP) & 1))
                b::smooth += 0.1f;
            if ((GetAsyncKeyState(VK_DOWN) & 1))
                b::smooth -= 0.1f;
            if ((GetAsyncKeyState(VK_RIGHT) & 1))
                b::fov += 2;
            if ((GetAsyncKeyState(VK_LEFT) & 1))
                b::fov -= 2;

            if ((GetAsyncKeyState(VK_RSHIFT) & 1))
                b::aimprority++;

            if (b::aimprority > 2)
                b::aimprority = 0;

            if ((GetAsyncKeyState(VK_RCONTROL) & 1))
                b::nospreadnorekoil = !b::nospreadnorekoil;
        }

        static int delayer = 0;
        delayer++;
        if (delayer >= 6) //so like 300ms
        {      
            std::rotate(std::begin(grogtext), std::next(std::begin(grogtext)), std::end(grogtext));
            delayer = 0;
        }

        SetConsoleTitleA(std::format("{} fov {:.1f}, smoothing {:.1f}, bone: {} :)", grogtext, b::fov, b::smooth, aimtypes[b::aimprority]).data());
        b::fov = clomp(b::fov, 1.f, 300.f);
        b::smooth = clomp(b::smooth, 1.f, 20.f);
               
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}