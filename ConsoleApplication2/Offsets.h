#pragma once
namespace Offsets
{
	extern uint64_t ObfuscationMgr;
	extern uint64_t ClientGameContext;
	extern uint64_t GameRender;


	//ClientPlayer 
	extern DWORD name;
	extern DWORD teamId;
	extern DWORD clientVehicleEntity;
	extern DWORD clientSoldierEntity;

	//ClientPlayerManager
	extern DWORD ClientPlayerManager = 0x0068;

	//ClientVehicleEntity 
	extern DWORD Vehiclehealthcomponent;
	extern DWORD EntityData;
	//ClientSoldierEntity 
	extern DWORD healthcomponent;
	extern DWORD bonecollisioncomponent;
	extern DWORD authorativeYaw;
	extern DWORD poseType;
	extern DWORD occluded;
	extern DWORD location;

	//UpdatePoseResultData 
	extern DWORD ActiveWorldTransforms;

	//BoneCollisionComponent 
	extern DWORD ragdollTransforms;

	//RenderView 
	extern DWORD viewMatrixInverse;
	extern DWORD viewProj;

	//QuatTransform
	extern DWORD validTransforms;
}

enum BoneIds {
	BONE_Head = 0x35,
	BONE_Neck = 0x33,
	BONE_SPINE2 = 0x7,
	BONE_SPINE1 = 0x6,
	BONE_Spine = 0x5,
	BONE_LeftShoulder = 0x8,
	BONE_RightShoulder = 0xA3,
	BONE_LeftElbowRoll = 0xE,
	BONE_RightElbowRoll = 0xA9,
	BONE_LeftHand = 0x10,
	BONE_RightHand = 0xAB,
	BONE_RightKneeRoll = 0x11F,
	BONE_LeftKneeRoll = 0x12D,
	BONE_LeftFoot = 0x115,
	BONE_RightFoot = 0x123
};