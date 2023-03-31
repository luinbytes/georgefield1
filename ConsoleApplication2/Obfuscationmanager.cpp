#include "Includes.h"
uint64_t EncryptedPlayerMgr_GetPlayer(const Memory& mem, uint64_t ptr, int id)
{
    uint64_t XorValue1 = mem.Read<uint64_t>(ptr + 0x20) ^ mem.Read<uint64_t>(ptr + 0x8);
    uint64_t XorValue2 = XorValue1 ^ mem.Read<uint64_t>(ptr + 0x10);
    if (!ValidPointer(XorValue2))
    {
        return 0;
    }

    return XorValue1 ^ mem.Read<uint64_t>(XorValue2 + 0x8 * id);
}

uint64_t GetPlayerById(const Memory& mem, int id)
{
    uint64_t pClientGameContext = mem.Read<uint64_t>(Offsets::ClientGameContext);
    if (!ValidPointer(pClientGameContext))
    {
        return 0;
    }

    uint64_t pPlayerManager = mem.Read<uint64_t>(pClientGameContext + 0x68);
    if (!ValidPointer(pPlayerManager))
    {
        return 0;
    }

    uint64_t pObfuscationMgr = mem.Read<uint64_t>(Offsets::ObfuscationMgr);
    if (!ValidPointer(pObfuscationMgr))
    {
        return 0;
    }

    uint64_t PlayerListXorValue = mem.Read<uint64_t>(pPlayerManager + 0xF8);
    uint64_t PlayerListKey = PlayerListXorValue ^ mem.Read<uint64_t>(pObfuscationMgr + 0x70);

    uint64_t mpBucketArray = mem.Read<uint64_t>(pObfuscationMgr + 0x10);


    int mnBucketCount = mem.Read<int>(pObfuscationMgr + 0x18);
    if (mnBucketCount == 0)
    {
        return 0;
    }

    int startCount = (int)PlayerListKey % mnBucketCount;

    uint64_t mpBucketArray_startCount = mem.Read<uint64_t>(mpBucketArray + (uint64_t)(startCount * 8));
    uint64_t node_first = mem.Read<uint64_t>(mpBucketArray_startCount);
    uint64_t node_second = mem.Read<uint64_t>(mpBucketArray_startCount + 0x8);
    uint64_t node_mpNext = mem.Read<uint64_t>(mpBucketArray_startCount + 0x10);

    while (PlayerListKey != node_first)
    {
        mpBucketArray_startCount = node_mpNext;

        node_first = mem.Read<uint64_t>(mpBucketArray_startCount);
        node_second = mem.Read<uint64_t>(mpBucketArray_startCount + 0x8);
        node_mpNext = mem.Read<uint64_t>(mpBucketArray_startCount + 0x10);
    }

    uint64_t EncryptedPlayerMgr = node_second;
    return EncryptedPlayerMgr_GetPlayer(EncryptedPlayerMgr, id);
}

uint64_t GetLocalPlayer(const Memory& mem)
{
    uint64_t pClientGameContext = mem.Read<uint64_t>(Offsets::ClientGameContext);
    if (!ValidPointer(pClientGameContext))
    {
        return 0;
    }

    uint64_t pPlayerManager = mem.Read<uint64_t>(pClientGameContext + 0x68);
    if (!ValidPointer(pPlayerManager))
    {
        return 0;
    }

    uint64_t pObfuscationMgr = mem.Read<uint64_t>(Offsets::ObfuscationMgr);
    if (!ValidPointer(pObfuscationMgr))
    {
        return 0;
    }

    uint64_t PlayerListXorValue = mem.Read<uint64_t>(pPlayerManager + 0xF8);
    uint64_t PlayerListKey = PlayerListXorValue ^ mem.Read<uint64_t>(pObfuscationMgr + 0x70);

    uint64_t mpBucketArray = mem.Read<uint64_t>(pObfuscationMgr + 0x10);


    int mnBucketCount = mem.Read<int>(pObfuscationMgr + 0x18);
    if (mnBucketCount == 0)
    {
        return 0;
    }

    int startCount = (int)PlayerListKey % mnBucketCount;

    uint64_t mpBucketArray_startCount = mem.Read<uint64_t>(mpBucketArray + (uint64_t)(startCount * 8));
    uint64_t node_first = mem.Read<uint64_t>(mpBucketArray_startCount);
    uint64_t node_second = mem.Read<uint64_t>(mpBucketArray_startCount + 0x8);
    uint64_t node_mpNext = mem.Read<uint64_t>(mpBucketArray_startCount + 0x10);

    while (PlayerListKey != node_first)
    {
        mpBucketArray_startCount = node_mpNext;

        node_first = mem.Read<uint64_t>(mpBucketArray_startCount);
        node_second = mem.Read<uint64_t>(mpBucketArray_startCount + 0x8);
        node_mpNext = mem.Read<uint64_t>(mpBucketArray_startCount + 0x10);
    }

    uint64_t EncryptedPlayerMgr = node_second;
    return EncryptedPlayerMgr_GetPlayer(EncryptedPlayerMgr, NULL);

}
uint64_t GetSoldierWeapon(const Memory& mem)
{
    DWORD64 pSoldierWeapon = 0x0;

    uint64_t pLocalPlayer = GetLocalPlayer();
    if (ValidPointer(pLocalPlayer)) {
        uint64_t pLocalSoldier = mem.Read<uint64_t>(pLocalPlayer + 0x1D48);
        if (ValidPointer(pLocalSoldier)) {
          
            uint64_t pClientWeaponComponent = mem.Read<uint64_t>(pLocalPlayer + 0x698);
                if (ValidPointer(pClientWeaponComponent)) {

                    uint64_t m_handler = mem.Read<uint64_t>(pClientWeaponComponent + 0x08A8);
                    uint64_t m_activeSlot = mem.Read<uint64_t>(pClientWeaponComponent + 0x0A10);

                    if (ValidPointer(m_handler))
                    {
                        pSoldierWeapon = mem.Read<uint64_t>(m_handler + m_activeSlot * 0x8);
                    }
                }
            
        }
    }
    return pSoldierWeapon;
}


//void* DecryptPointer(DWORD64 EncryptedPtr, DWORD64 PointerKey)
//{
//    _QWORD pObfuscationMgr = *(_QWORD*)OFFSET_ObfuscationMgr;
//
//    if (!ValidPointer(pObfuscationMgr))
//        return nullptr;
//
//    if (!(EncryptedPtr & 0x8000000000000000))
//        return nullptr; //invalid ptr
//
//    _QWORD hashtableKey = PointerKey ^ *(_QWORD*)(pObfuscationMgr + 0x70);
//
//    hashtable<_QWORD>* table = (hashtable<_QWORD>*)(pObfuscationMgr + 0x38);
//    hashtable_iterator<_QWORD> iterator = {};
//
//    hashtable_find(table, &iterator, hashtableKey);
//    if (iterator.mpNode == table->mpBucketArray[table->mnBucketCount])
//        return nullptr;
//
//    _QWORD EncryptionKey = hashtableKey ^ (_QWORD)(iterator.mpNode->mValue.second);
//    EncryptionKey ^= (7 * EncryptionKey);
//
//    _QWORD DecryptedPtr = NULL;
//    BYTE* pDecryptedPtrBytes = (BYTE*)&DecryptedPtr;
//    BYTE* pEncryptedPtrBytes = (BYTE*)&EncryptedPtr;
//    BYTE* pKeyBytes = (BYTE*)&EncryptionKey;
//
//    for (char i = 0; i < 7; i++)
//    {
//        pDecryptedPtrBytes[i] = (pKeyBytes[i] * 0x7A) ^ (pEncryptedPtrBytes[i] + pKeyBytes[i]);
//        EncryptionKey += 8;
//    }
//    pDecryptedPtrBytes[7] = pEncryptedPtrBytes[7];
//
//    DecryptedPtr &= ~(0x8000000000000000); //to exclude the check bit
//
//    return (void*)DecryptedPtr;
//}