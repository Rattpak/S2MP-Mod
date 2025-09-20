///////////////////////////////////////////
//         Arxan Patches
//	  Gettin rid of anti-tamper
////////////////////////////////////////////
#include "pch.h"
#include "Arxan.hpp"
#include <sstream>
#include <iomanip> 

std::string ArxanPatches::conLabel = "ARX";

std::vector<intactChecksumHook> intactchecksumHooks;
std::vector<intactBigChecksumHook> intactBigchecksumHooks;
std::vector<splitChecksumHook> splitchecksumHooks;

uint64_t int2dList[] =
{
    0xDA9, //double check this one
    0x76EB,
    0xB2C6,
    0x98038,
};

//hardcoding to save patching time
std::vector<uint64_t> integrityIntact =
{
    0x2292E5,
    0x114FEC1E,
    0x1150A60F,
    0x11619C9C,
    0x11645E6F,
    0x1164F9B7,
    0x11666AB5,
    0x1167DCD7,
    0x1172EC75,
    0x11850023,
    0x1186749D,
    0x11876212,
    0x11A6583D,
    0x11A9D356,
    0x11AA7C59,
    0x11B08E3B,
    0x11B1471B,
    0x11B63974,
    0x11B6FA21,
    0x11B712B2,
    0x11BD4EDF,
    0x11BE5FA6,
    0x11C2BB20,
    0x11C2D93E,
    0x11CBD297,
    0x11D9D8CC,
    0x11D9E481,
    0x11F93A1F,
    0x11F93B8E,
    0x11F94017,
    0x12274582,
    0x122DCCDA,
    0x122DF3C3,
    0x1230491D,
    0x1236A2DE,
    0x1237A405,
    0x1246C5AA,
    0x124A0507,
    0x124AA149,
    0x124E4C2C,
    0x125325A0,
    0x12533083,
    0x125F55EC,
    0x12615774,
    0x12796258,
    0x128D21AA,
    0x1297C5E1,
    0x129CB98E,
    0x12DA1CBB,
    0x12DEF14B,
};

//hardcoding to save patching time
std::vector<uint64_t> integrityIntactBig =
{
    0xDCEF,
    0x114CF343,
    0x114E105E,
    0x1154E03E,
    0x1157C195,
    0x1157D923,
    0x115877C0,
    0x1167027B,
    0x1167268C,
    0x1167EA85,
    0x11794887,
    0x117F374A,
    0x1184FF94,
    0x11855F3C,
    0x11861969,
    0x1186A7B0,
    0x11872CFA,
    0x11873FC9,
    0x1187491E,
    0x11A3272D,
    0x11A82A71,
    0x11A86245,
    0x11AA894E,
    0x11B6159E,
    0x11BD7FEF,
    0x11BDA1F0,
    0x11BE580E,
    0x11BFCC5A,
    0x11C1087C,
    0x11C15DB4,
    0x11CC0538,
    0x11CC20C7,
    0x11CCC580,
    0x11F99C4D,
    0x11F9CC96,
    0x122741F5,
    0x122F30BD,
    0x12447E76,
    0x12489BD9,
    0x124E4235,
    0x1252AF85,
    0x1253177A,
    0x125CB71E,
    0x125F6441,
    0x12627AE1,
    0x12627C6C,
    0x12632E12,
    0x1263774E,
    0x128CCEF1,
    0x128DB064,
    0x12931539,
    0x12940FAF,
    0x1296401C,
    0x129693A3,
    0x12D9CC32,
    0x12DBCFA6,
    0x12E002C1,
 
};

//hardcoding to save patching time
std::vector<uint64_t> integritySplit =
{
    0x11679725,
    0x1179EF4A,
    0x118518C2,
    0x1186615C,
    0x11A32814,
    0x11A5B061,
    0x11A774D1,
    0x11A79293,
    0x11AFFF99,
    0x11B59810,
    0x11CA40BC,
    0x11CE4D91,
    0x11F99582,
    0x11FA4952,
    0x122C24D5,
    0x122C5E5F,
    0x1243C7D7,
    0x125EE67F,
    0x128E119C,
    0x1297140A,
};

inlineAsmStub* inlineStubs = nullptr;
size_t stubCounter = 0;

uint32_t reverse_bytes(uint32_t bytes)
{
    uint32_t aux = 0;
    uint8_t byte;
    int i;

    for (i = 0; i < 32; i += 8)
    {
        byte = (bytes >> i) & 0xff;
        aux |= byte << (32 - 8 - i);
    }
    return aux;
}


bool ArxanPatches::is_relatively_far(const void* pointer, const void* data) {
    const int64_t diff = size_t(data) - (size_t(pointer) + 5);
    const auto small_diff = int32_t(diff);
    return diff != int64_t(small_diff);
}

uint8_t* ArxanPatches::allocate_somewhere_near(const void* base_address, const size_t size) {
    size_t offset = 0;
    while (true) {
        offset += size;
        auto* target_address = static_cast<const uint8_t*>(base_address) - offset;
        if (is_relatively_far(base_address, target_address)) {
            return nullptr;
        }

        const auto res = VirtualAlloc(const_cast<uint8_t*>(target_address), size, MEM_RESERVE | MEM_COMMIT,
            PAGE_EXECUTE_READWRITE);
        if (res) {
            if (is_relatively_far(base_address, target_address))
            {
                VirtualFree(res, 0, MEM_RELEASE);
                return nullptr;
            }

            return static_cast<uint8_t*>(res);
        }
    }
}

int FixChecksum(uint64_t rbpOffset, uint64_t ptrOffset, uint64_t* ptrStack, uint32_t jmpInstructionDistance, uint32_t calculatedChecksumFromArg)
{
    // get size of image from codcw
    uint64_t baseAddressStart = (uint64_t)GetModuleHandle(nullptr);
    IMAGE_DOS_HEADER* pDOSHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
    IMAGE_NT_HEADERS* pNTHeaders = (IMAGE_NT_HEADERS*)((BYTE*)pDOSHeader + pDOSHeader->e_lfanew);
    auto sizeOfImage = pNTHeaders->OptionalHeader.SizeOfImage;
    uint64_t baseAddressEnd = baseAddressStart + sizeOfImage;

    // check if our checksum hooks got overwritten
    // we could probably ifdef this now but it's a good indicator to know if our checksum hooks still exist
    {
        for (int i = 0; i < intactchecksumHooks.size(); i++)
        {
            DWORD old_protect{};

            VirtualProtect(intactchecksumHooks[i].functionAddress, sizeof(uint8_t) * 7, PAGE_EXECUTE_READWRITE, &old_protect);
            memcpy(intactchecksumHooks[i].functionAddress, intactchecksumHooks[i].buffer, sizeof(uint8_t) * 7);
            VirtualProtect(intactchecksumHooks[i].functionAddress, sizeof(uint8_t) * 7, old_protect, &old_protect);
            FlushInstructionCache(GetCurrentProcess(), intactchecksumHooks[i].functionAddress, sizeof(uint8_t) * 7);
        }

        for (int i = 0; i < intactBigchecksumHooks.size(); i++)
        {
            DWORD old_protect{};

            VirtualProtect(intactBigchecksumHooks[i].functionAddress, sizeof(uint8_t) * 10, PAGE_EXECUTE_READWRITE, &old_protect);
            memcpy(intactBigchecksumHooks[i].functionAddress, intactBigchecksumHooks[i].buffer, sizeof(uint8_t) * 10);
            VirtualProtect(intactBigchecksumHooks[i].functionAddress, sizeof(uint8_t) * 10, old_protect, &old_protect);
            FlushInstructionCache(GetCurrentProcess(), intactBigchecksumHooks[i].functionAddress, sizeof(uint8_t) * 10);
        }

        for (int i = 0; i < splitchecksumHooks.size(); i++)
        {
            DWORD old_protect{};

            VirtualProtect(splitchecksumHooks[i].functionAddress, sizeof(uint8_t) * 8, PAGE_EXECUTE_READWRITE, &old_protect);
            memcpy(splitchecksumHooks[i].functionAddress, splitchecksumHooks[i].buffer, sizeof(uint8_t) * 8);
            VirtualProtect(splitchecksumHooks[i].functionAddress, sizeof(uint8_t) * 8, old_protect, &old_protect);
            FlushInstructionCache(GetCurrentProcess(), splitchecksumHooks[i].functionAddress, sizeof(uint8_t) * 8);
        }
    }

    static int fixChecksumCalls = 0;
    fixChecksumCalls++;

    uint32_t calculatedChecksum = calculatedChecksumFromArg;
    uint32_t reversedChecksum = reverse_bytes(calculatedChecksumFromArg);
    uint32_t* calculatedChecksumPtr = (uint32_t*)((char*)ptrStack + 0x120); // 0x120 is a good starting point to decrement downwards to find the calculated checksum on the stack
    uint32_t* calculatedReversedChecksumPtr = (uint32_t*)((char*)ptrStack + 0x120); // 0x120 is a good starting point to decrement downwards to find the calculated checksum on the stack

    bool doubleTextChecksum = false;
    uint64_t* previousResultPtr = nullptr;
    if (ptrOffset == 0 && rbpOffset < 0x90)
    {
        uint64_t* textPtr = (uint64_t*)((char*)ptrStack + rbpOffset + (rbpOffset % 0x8)); // make sure rbpOffset is aligned by 8 bytes
        int pointerCounter = 0;

        for (int i = 0; i < 20; i++)
        {
            uint64_t derefPtr = *(uint64_t*)textPtr;

            if (derefPtr >= baseAddressStart && derefPtr <= baseAddressEnd)
            {
                uint64_t derefResult = **(uint64_t**)textPtr;
                pointerCounter++;

                // store the ptr above 0xffffffffffffffff and then use it in our originalchecksum check
                if (derefResult == 0xffffffffffffffff)
                {
                    if (pointerCounter > 2)
                    {
                        doubleTextChecksum = true;

                        // because textptr will be pointing at 0xffffffffffffffff, increment it once 
                        // so we are pointing to the correct checksum location

                        // TODO: remove this, doesnt do anything, confirm with checksum 0x79d397c8
                        // since we use previousResultPtr which doesnt rely on this
                        textPtr++;
                    }

                    break;
                }

                previousResultPtr = textPtr;
            }

            textPtr--;
        }
    }
    else
    {	// for debugging stack traces on bigger rbp offset checksums
        uint64_t* textPtr = (uint64_t*)((char*)ptrStack + rbpOffset + (rbpOffset % 0x8)); // make sure rbpOffset is aligned by 8 bytes

        for (int i = 0; i < 30; i++)
        {
            uint64_t derefPtr = *(uint64_t*)textPtr;

            if (derefPtr >= baseAddressStart && derefPtr <= baseAddressEnd)
                uint64_t derefResult = **(uint64_t**)textPtr;

            textPtr--;
        }
    }

    // find calculatedChecksumPtr, we will overwrite this later with the original checksum
    for (int i = 0; i < 80; i++)
    {
        uint32_t derefPtr = *(uint32_t*)calculatedChecksumPtr;

        if (derefPtr == calculatedChecksum)
            break;

        calculatedChecksumPtr--;
    }

    // find calculatedReversedChecksumPtr, we will overwrite this later with the original checksum
    for (int i = 0; i < 80; i++)
    {
        uint32_t derefPtr = *(uint32_t*)calculatedReversedChecksumPtr;

        if (derefPtr == reversedChecksum)
            break;

        calculatedReversedChecksumPtr--;
    }

    uint64_t* textPtr = (uint64_t*)((char*)ptrStack + rbpOffset + (rbpOffset % 0x8)); // add remainder to align ptr
    uint32_t originalChecksum = NULL;
    uint32_t* originalChecksumPtr = nullptr;

    // searching for a .text pointer that points to the original checksum, upwards from the rbp	
    for (int i = 0; i < 10; i++)
    {
        uint64_t derefPtr = *(uint64_t*)textPtr;

        if (derefPtr >= baseAddressStart && derefPtr <= baseAddressEnd)
        {
            if (ptrOffset == 0 && rbpOffset < 0x90)
            {
                if (doubleTextChecksum)
                    originalChecksum = **(uint32_t**)previousResultPtr;
                else
                    originalChecksum = *(uint32_t*)derefPtr;
            }
            else
            {
                originalChecksum = *(uint32_t*)((char*)derefPtr + ptrOffset * 4); // if ptrOffset is used the original checksum is in a different spot
                originalChecksumPtr = (uint32_t*)((char*)derefPtr + ptrOffset * 4);
            }

            break;
        }

        textPtr--;
    }

    *calculatedChecksumPtr = (uint32_t)originalChecksum;
    *calculatedReversedChecksumPtr = reverse_bytes((uint32_t)originalChecksum);

    // for big intact we need to keep overwriting 4 more times
    // seems to still run even if we comment this out wtf?
    uint32_t* tmpOriginalChecksumPtr = originalChecksumPtr;
    uint32_t* tmpCalculatedChecksumPtr = calculatedChecksumPtr;
    uint32_t* tmpReversedChecksumPtr = calculatedReversedChecksumPtr;
    if (originalChecksumPtr != nullptr)
    {
        for (int i = 0; i <= ptrOffset; i++)
        {
            *tmpCalculatedChecksumPtr = *(uint32_t*)tmpOriginalChecksumPtr;
            *tmpReversedChecksumPtr = reverse_bytes(*(uint32_t*)tmpOriginalChecksumPtr);

            tmpOriginalChecksumPtr--;
            tmpCalculatedChecksumPtr--;
            tmpReversedChecksumPtr--;
        }
    }

    return originalChecksum;
}

bool arxanHealingChecksum(uint64_t rbp)
{
    // check if rbpAddressLocationPtr is within the range of 8 bytes up & down from every checksum that we placed.
    uint64_t rbpAddressLocationPtr = *(uint64_t*)(rbp + 0x10);

    for (int i = 0; i < stubCounter; i++)
    {
        // 0x8
        // TODO: if 0x7 is too big then "mov [rdx], al" will make the game crash probably because its trying to overwrite areas next to our hooks that have to get modified.
        // we could do two seperate functions since "mov [rdx], eax" would be a 32 byte offset (?) and "mov [rdx], al" would be 4 byte offset (?)

        if (rbpAddressLocationPtr + 0x7 >= (uint64_t)inlineStubs[i].functionAddress &&
            rbpAddressLocationPtr - 0x7 <= (uint64_t)inlineStubs[i].functionAddress)
        {
            return true;
        }
    }

    return false;
}

void CreateChecksumHealingStub()
{
    void* baseModule = GetModuleHandle(nullptr);

    checksumHealingLocation healingLocations[]{
        {hook::module_pattern(baseModule, "89 02 8B 45 20"), 5},
        {hook::module_pattern(baseModule, "88 02 83 45 20 FF"), 6},
        {hook::module_pattern(baseModule, "89 02 E9"), 7},
        {hook::module_pattern(baseModule, "88 02 E9"), 7},
    };

    const size_t allocationSize = sizeof(uint8_t) * 0x100 * 1000;
    LPVOID healingStubLocation = ArxanPatches::allocate_somewhere_near(GetModuleHandle(nullptr), allocationSize);
    memset(healingStubLocation, 0x90, allocationSize);

    // avoid stub generation collision
    char* previousStubOffset = nullptr;
    // for jmp distance calculation
    char* currentStubOffset = nullptr;

    size_t amountOfPatterns = sizeof(healingLocations) / sizeof(checksumHealingLocation);
    for (int type = 0; type < amountOfPatterns; type++)
    {
        size_t locations = healingLocations[type].checksumPattern.size();
        for (int i = 0; i < locations; i++)
        {
            uint8_t instructionBuffer[4] = {}; // 88 02 E9: 4          
            int32_t jumpDistance = 0;
            size_t callInstructionOffset = 5; // 0xE8 ? ? ? ?
            uint64_t jumpInstruction;
            uint64_t locationToJump;

            // we don't know the previous offset yet
            if (currentStubOffset == nullptr)
                currentStubOffset = (char*)healingStubLocation;

            if (previousStubOffset != nullptr)
                currentStubOffset = previousStubOffset;

            void* functionAddress = healingLocations[type].checksumPattern.get(i).get<void*>(0);

            if (*(uint8_t*)((uint8_t*)functionAddress + 2) == 0xe9)
            {
                memcpy(&jumpDistance, (char*)functionAddress + 3, 4); // ptr after 0xE9
                jumpInstruction = (uint64_t)functionAddress + 2; 		// at the jmp instruction
                locationToJump = jumpInstruction + jumpDistance + callInstructionOffset;

                // get size of image from codcw
                uint64_t baseAddressStart = (uint64_t)GetModuleHandle(nullptr);
                IMAGE_DOS_HEADER* pDOSHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
                IMAGE_NT_HEADERS* pNTHeaders = (IMAGE_NT_HEADERS*)((BYTE*)pDOSHeader + pDOSHeader->e_lfanew);
                auto sizeOfImage = pNTHeaders->OptionalHeader.SizeOfImage;
                uint64_t baseAddressEnd = baseAddressStart + sizeOfImage;

                if ((locationToJump > baseAddressStart && locationToJump < baseAddressEnd) != true)
                    continue;

                memcpy(instructionBuffer, (char*)locationToJump, sizeof(uint8_t) * 4);

                if (type == 2)
                {
                    uint8_t instruction[3] = { 0x8B, 0x45, 0x20 };
                    if (memcmp(instructionBuffer, instruction, sizeof(uint8_t) * 3) != 0)
                        continue;
                }

                if (type == 3)
                {
                    uint8_t instruction[4] = { 0x83, 0x45, 0x20, 0xFF };
                    if (memcmp(instructionBuffer, instruction, sizeof(uint8_t) * 4) != 0)
                        continue;
                }
            }

            static asmjit::JitRuntime runtime;
            asmjit::CodeHolder code;
            code.init(runtime.environment());

            using namespace asmjit::x86;
            Assembler a(&code);
            asmjit::Label L1 = a.newLabel();
            asmjit::Label DEBUG = a.newLabel();

            a.sub(rsp, 0x32);
            pushad64_Min();

            a.mov(rcx, rbp);
            a.mov(r15, (uint64_t)(void*)arxanHealingChecksum);
            a.call(r15);
            a.movzx(r15, al);	// if arxan tries to replace our checksum set r15 to 1

            popad64_Min();
            a.add(rsp, 0x32);

            switch (type)
            {
            case 0:
                /*
                    mov     [rdx], eax
                    mov     eax, [rbp+20h]
                */
                // dont replace our checksum if r15 is 1
                a.cmp(r15, 1);
                a.je(L1);
                a.mov(qword_ptr(rdx), eax);

                a.bind(L1);
                a.mov(eax, qword_ptr(rbp, 0x20));
                a.ret();
                break;
            case 1:
                /*
                    mov     [rdx], al
                    add     dword ptr [rbp+20h], -1
                */
                // dont replace our checksum if r15 is 1
                a.cmp(r15, 1);
                a.je(L1);
                a.mov(qword_ptr(rdx), al);

                a.bind(L1);
                a.add(dword_ptr(rbp, 0x20), -1);
                a.ret();
                break;
            case 2:
                /*
                    mov     [rdx], eax
                    jmp     loc_7FF7366C7B94
                */
                // dont replace our checksum if r15 is 1
                a.cmp(r15, 1);
                a.je(L1);
                a.mov(qword_ptr(rdx), eax);

                a.bind(L1);
                a.add(rsp, 0x8);
                a.mov(r15, locationToJump);
                a.push(r15);
                a.ret();
                break;
            case 3:
                /*
                    mov     [rdx], al
                    jmp     loc_7FF738FB7A45
                */
                // dont replace our checksum if r15 is 1
                a.cmp(r15, 1);
                a.je(L1);
                a.mov(qword_ptr(rdx), al);

                a.bind(L1);
                a.add(rsp, 0x8);
                a.mov(r15, locationToJump);
                a.push(r15);
                a.ret();
                break;
            default:
                Console::devPrint("Error: We shouldn't be here");
                getchar();
                abort();
            }

            void* asmjitResult = nullptr;
            runtime.add(&asmjitResult, &code);

            // copy over the content to the stub
            uint8_t* tempBuffer = (uint8_t*)malloc(sizeof(uint8_t) * code.codeSize());
            memcpy(tempBuffer, asmjitResult, code.codeSize());
            memcpy(currentStubOffset, tempBuffer, sizeof(uint8_t) * code.codeSize());

            size_t callInstructionBytes = healingLocations[type].length;
            size_t callInstructionLength = sizeof(uint8_t) * callInstructionBytes;

            DWORD old_protect{};
            VirtualProtect(functionAddress, callInstructionLength, PAGE_EXECUTE_READWRITE, &old_protect);
            memset(functionAddress, 0, callInstructionLength);
            VirtualProtect(functionAddress, callInstructionLength, old_protect, &old_protect);
            FlushInstructionCache(GetCurrentProcess(), functionAddress, callInstructionLength);

            uint64_t jmpDistance = (uint64_t)currentStubOffset - (uint64_t)functionAddress - 5;
            uint8_t* jmpInstructionBuffer = (uint8_t*)malloc(sizeof(uint8_t) * callInstructionBytes);

            // E8 cd CALL rel32  Call near, relative, displacement relative to next instruction
            jmpInstructionBuffer[0] = 0xE8;
            jmpInstructionBuffer[1] = (jmpDistance >> (0 * 8));
            jmpInstructionBuffer[2] = (jmpDistance >> (1 * 8));
            jmpInstructionBuffer[3] = (jmpDistance >> (2 * 8));
            jmpInstructionBuffer[4] = (jmpDistance >> (3 * 8));

            for (int v = 0; v < callInstructionBytes - 5; v++)
                jmpInstructionBuffer[5 + v] = 0x90;

            VirtualProtect(functionAddress, callInstructionLength, PAGE_EXECUTE_READWRITE, &old_protect);
            memcpy(functionAddress, jmpInstructionBuffer, callInstructionLength);
            VirtualProtect(functionAddress, callInstructionLength, old_protect, &old_protect);
            FlushInstructionCache(GetCurrentProcess(), functionAddress, callInstructionLength);

            previousStubOffset = currentStubOffset + sizeof(uint8_t) * code.codeSize() + 0x8;

#ifdef ARXAN_DEBUG_INFO
            if (i == 0) {
                std::ostringstream oss;
                oss << "Patched all Heal type: " << type << " 0x" << std::hex << functionAddress << "\n";
                Console::labelPrint(ArxanPatches::conLabel, oss.str());
            }
#endif // ARXAN_DEBUG_INFO
            
        }
    }
#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(ArxanPatches::conLabel, std::string(__FUNCTION__) + " complete");
#endif // ARXAN_DEBUG_INFO
}

void createInlineAsmStub() {
    int integIntactCount = integrityIntact.size();
    int integIntactBigCount = integrityIntactBig.size();
    int integSplitCount = integritySplit.size();
    int totalIntegrityCount = integIntactCount + integIntactBigCount + integSplitCount;

#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(ArxanPatches::conLabel, "Total Integrity Check Count: " + std::to_string(totalIntegrityCount));
#endif // ARXAN_DEBUG_INFO

    //based on CWHook
    const size_t allocationSize = sizeof(uint8_t) * 128;
    inlineStubs = (inlineAsmStub*)malloc(sizeof(inlineAsmStub) * totalIntegrityCount);
    if (!inlineStubs) {
        Console::devPrint("ERROR: Failed malloc in " + std::string(__FUNCTION__));
        return;
    }

    for (int i = 0; i < integIntactCount; i++) {
        inlineStubs[stubCounter].functionAddress = reinterpret_cast<void*>(integrityIntact[i] + GameUtil::base);
        inlineStubs[stubCounter].type = intactSmall;
        inlineStubs[stubCounter].bufferSize = 7;
        stubCounter++;
    }

#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(ArxanPatches::conLabel, "Finished setting up integrityIntact");
#endif // ARXAN_DEBUG_INFO

    for (int i = 0; i < integIntactBigCount; i++) {
        inlineStubs[stubCounter].functionAddress = reinterpret_cast<void*>(integrityIntactBig[i] + GameUtil::base);
        inlineStubs[stubCounter].type = intactBig;
        inlineStubs[stubCounter].bufferSize = 10;
        stubCounter++;
    }

#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(ArxanPatches::conLabel, "Finished setting up integrityIntactBig");
#endif // ARXAN_DEBUG_INFO

    for (int i = 0; i < integSplitCount; i++) {
        inlineStubs[stubCounter].functionAddress = reinterpret_cast<void*>(integritySplit[i] + GameUtil::base);
        inlineStubs[stubCounter].type = split;
        inlineStubs[stubCounter].bufferSize = 8;
        stubCounter++;
    }

#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(ArxanPatches::conLabel, "Finished setting up integritySplit");
#endif // ARXAN_DEBUG_INFO

    LPVOID asmBigStubLocation = ArxanPatches::allocate_somewhere_near(GetModuleHandle(nullptr), allocationSize * 0x80);
    memset(asmBigStubLocation, 0x90, allocationSize * 0x80);

    char* previousStubOffset = nullptr;
    char* currentStubOffset = nullptr;

    ///////////////////////////////////////////////////////////
    for (int i = 0; i < stubCounter; i++) {
        // we don't know the previous offset yet
        if (currentStubOffset == nullptr)
            currentStubOffset = (char*)asmBigStubLocation;

        if (previousStubOffset != nullptr)
            currentStubOffset = previousStubOffset;

        void* functionAddress = inlineStubs[i].functionAddress;
        uint64_t jmpDistance = (uint64_t)currentStubOffset - (uint64_t)functionAddress - 5; // 5 bytes from relative call instruction


        // backup instructions that will get destroyed
        const int length = sizeof(uint8_t) * 8;

        uint8_t instructionBuffer[8] = {};

        uint64_t base = (uintptr_t)GetModuleHandle(NULL);
        uint64_t offset = 0;

        if (i < integrityIntact.size()) {
            offset = integrityIntact[i] - base;
        }
        else {
            offset = integrityIntactBig[i - integrityIntact.size()] - base;
        }

        //std::stringstream ss;
        //ss << "Index: " << i
        //    << ", Base: 0x" << std::hex << std::setw(16) << std::setfill('0') << base
        //    << ", Offset: 0x" << std::hex << offset
        //    << ", Function Address: 0x" << std::hex << std::setw(16) << std::setfill('0') << (uint64_t)functionAddress;

         //Console::devPrint(ss.str());

        if (!IsBadReadPtr(functionAddress, length)) {
            memcpy(instructionBuffer, functionAddress, length);
        }
        else {
            Console::devPrint("ERROR: Invalid function address at index " + std::to_string(i));
            continue;
        }

        uint32_t instructionBufferJmpDistance = 0;
        if (instructionBuffer[3] == 0xE9)
            memcpy(&instructionBufferJmpDistance, (char*)functionAddress + 0x4, 4); // 0x4 so we skip 0xE9

        uint64_t rbpOffset = 0x0;
        bool jumpDistanceNegative = instructionBufferJmpDistance >> 31; // get sign bit from jump distance
        int32_t jumpDistance = instructionBufferJmpDistance;

        if (inlineStubs[i].type == split)
        {
            //Console::labelPrint(ArxanPatches::conLabel, "processing split");
            char* rbpOffsetPtr = nullptr;

            // TODO: just use jumpDistance once we got a working test case
            if (jumpDistanceNegative)
                rbpOffsetPtr = (char*)((uint64_t)functionAddress + jumpDistance + 0x8);
            else
                rbpOffsetPtr = (char*)((uint64_t)functionAddress + instructionBufferJmpDistance + 0x8);

            rbpOffsetPtr++;

            // depending on the rbp offset from add dword ptr we need one more byte for the rbpOffset
            if (*(unsigned char*)rbpOffsetPtr == 0x45) 		// add dword ptr [rbp+68],-01
            {
                rbpOffsetPtr++;
                rbpOffset = *(char*)rbpOffsetPtr;
            }
            else if (*(unsigned char*)rbpOffsetPtr == 0x85)	// add dword ptr [rbp+1CC],-01
            {
                rbpOffsetPtr++;
                rbpOffset = *(short*)rbpOffsetPtr;
            }
        }

        // create assembly stub content
        // TODO: we could create three different asmjit build sections for each type
        // so we don't have if statements inbetween instructions for the cost of LOC but it would be more readible
        static asmjit::JitRuntime runtime;
        asmjit::CodeHolder code;
        code.init(runtime.environment());
        asmjit::x86::Assembler a(&code);

        if (inlineStubs[i].type != split)
            rbpOffset = instructionBuffer[5];


        a.sub(asmjit::x86::rsp, 0x32);
        pushad64();

        a.mov(asmjit::x86::qword_ptr(asmjit::x86::rsp, 0x20), asmjit::x86::rax);
        a.mov(asmjit::x86::rdx, asmjit::x86::rcx);	// offset within text section pointer (ecx*4)

        // we dont use rbpoffset since we only get 1 byte from the 2 byte offset (rbpOffset)
        // 0x130 is a good starting ptr to decrement downwards so we can find the original checksum
        if (inlineStubs[i].type == intactBig)
            a.mov(asmjit::x86::rcx, 0x120);
        else
            a.mov(asmjit::x86::rcx, rbpOffset);

        a.mov(asmjit::x86::r8, asmjit::x86::rbp);


        if (inlineStubs[i].type == split)
        {
            if (jumpDistanceNegative)
                a.mov(asmjit::x86::r9, jumpDistance);
            else
                a.mov(asmjit::x86::r9, instructionBufferJmpDistance);
        }
        else
            a.mov(asmjit::x86::r9, instructionBufferJmpDistance);	// incase we mess up a split checksum

        a.mov(asmjit::x86::rax, (uint64_t)(void*)FixChecksum);
        a.call(asmjit::x86::rax);
        a.add(asmjit::x86::rsp, 0x8 * 4); // so that r12-r15 registers dont get corrupt

        popad64WithoutRAX();
        a.add(asmjit::x86::rsp, 0x32);

        a.mov(ptr(asmjit::x86::rdx, asmjit::x86::rcx, 2), asmjit::x86::eax); // mov [rdx+rcx*4], eax

        if (instructionBufferJmpDistance == 0) {
            if (inlineStubs[i].type == intactBig)
                rbpOffset += 0x100;

            a.add(dword_ptr(asmjit::x86::rbp, rbpOffset), -1); // add dword ptr [rbp+rbpOffset], 0FFFFFFFFh
        }
        else {
            // jmp loc_7FF641C707A5
            // push the desired address on to the stack and then perform a 64 bit RET
            a.add(asmjit::x86::rsp, 0x8); // pop return address off the stack cause we will jump
            uint64_t addressToJump = (uint64_t)functionAddress + instructionBufferJmpDistance;

            if (inlineStubs[i].type == split)
            {
                // TODO: just use jumpDistance once we got a working test case
                if (jumpDistanceNegative)
                    addressToJump = (uint64_t)functionAddress + jumpDistance + 0x8; // 0x8 call instruction + offset + 2 nops
                else
                    addressToJump = (uint64_t)functionAddress + instructionBufferJmpDistance + 0x8; // 0x8 call instruction + offset + 2 nops
            }

            a.mov(asmjit::x86::r11, addressToJump);	// r11 is being used but should be fine based on documentation

            if (inlineStubs[i].type == split)
                a.add(asmjit::x86::rsp, 0x8); // since we dont pop off rax we need to sub 0x8 the rsp

            a.push(asmjit::x86::r11);
        }

        if (inlineStubs[i].type != split)
            a.add(asmjit::x86::rsp, 0x8); // since we dont pop off rax we need to sub 0x8 the rsp

        a.ret();

        void* asmjitResult = nullptr;
        runtime.add(&asmjitResult, &code);

        // copy over the content to the stub
        uint8_t* tempBuffer = (uint8_t*)malloc(sizeof(uint8_t) * code.codeSize());
        memcpy(tempBuffer, asmjitResult, code.codeSize());
        memcpy(currentStubOffset, tempBuffer, sizeof(uint8_t) * code.codeSize());

        size_t callInstructionBytes = inlineStubs[i].bufferSize;
        size_t callInstructionLength = sizeof(uint8_t) * callInstructionBytes;

        DWORD old_protect{};
        VirtualProtect(functionAddress, callInstructionLength, PAGE_EXECUTE_READWRITE, &old_protect);
        memset(functionAddress, 0, callInstructionLength);
        VirtualProtect(functionAddress, callInstructionLength, old_protect, &old_protect);
        FlushInstructionCache(GetCurrentProcess(), functionAddress, callInstructionLength);

        // E8 cd CALL rel32  Call near, relative, displacement relative to next instruction
        uint8_t* jmpInstructionBuffer = (uint8_t*)malloc(sizeof(uint8_t) * callInstructionBytes);
        jmpInstructionBuffer[0] = 0xE8;
        jmpInstructionBuffer[1] = (jmpDistance >> (0 * 8));
        jmpInstructionBuffer[2] = (jmpDistance >> (1 * 8));
        jmpInstructionBuffer[3] = (jmpDistance >> (2 * 8));
        jmpInstructionBuffer[4] = (jmpDistance >> (3 * 8));
        jmpInstructionBuffer[5] = 0x90;
        jmpInstructionBuffer[6] = 0x90;

        if (inlineStubs[i].type == intactBig) {
            jmpInstructionBuffer[7] = 0x90;
            jmpInstructionBuffer[8] = 0x90;
            jmpInstructionBuffer[9] = 0x90;
        }

        if (inlineStubs[i].type == split)
            jmpInstructionBuffer[7] = 0x90;

        VirtualProtect(functionAddress, callInstructionLength, PAGE_EXECUTE_READWRITE, &old_protect);
        memcpy(functionAddress, jmpInstructionBuffer, callInstructionLength);
        VirtualProtect(functionAddress, callInstructionLength, old_protect, &old_protect);
        FlushInstructionCache(GetCurrentProcess(), functionAddress, callInstructionLength);

        // store location & bytes to check if arxan is removing our hooks
        if (inlineStubs[i].type == intactSmall) {
            intactChecksumHook intactChecksum = {};
            intactChecksum.functionAddress = (uint64_t*)functionAddress;
            memcpy(intactChecksum.buffer, jmpInstructionBuffer, sizeof(uint8_t) * inlineStubs[i].bufferSize);
            inlineStubs[i].buffer = intactChecksum.buffer;
            intactchecksumHooks.push_back(intactChecksum);
        }

        if (inlineStubs[i].type == intactBig) {
            intactBigChecksumHook intactBigChecksum = {};
            intactBigChecksum.functionAddress = (uint64_t*)functionAddress;
            memcpy(intactBigChecksum.buffer, jmpInstructionBuffer, sizeof(uint8_t) * inlineStubs[i].bufferSize);
            inlineStubs[i].buffer = intactBigChecksum.buffer;
            intactBigchecksumHooks.push_back(intactBigChecksum);
        }

        if (inlineStubs[i].type == split)
        {
            splitChecksumHook splitChecksum = {};
            splitChecksum.functionAddress = (uint64_t*)functionAddress;
            memcpy(splitChecksum.buffer, jmpInstructionBuffer, sizeof(uint8_t) * inlineStubs[i].bufferSize);
            inlineStubs[i].buffer = splitChecksum.buffer;
            splitchecksumHooks.push_back(splitChecksum);
        }

        previousStubOffset = currentStubOffset + sizeof(uint8_t) * code.codeSize() + 0x8;
    }

#ifdef ARXAN_DEBUG_INFO
    Console::labelPrint(ArxanPatches::conLabel, std::string(__FUNCTION__) + " complete");
    Console::labelPrint(ArxanPatches::conLabel, "integSplitCount: " + std::to_string(integSplitCount));
    Console::labelPrint(ArxanPatches::conLabel, "integIntactBigCount: " + std::to_string(integIntactBigCount));
    Console::labelPrint(ArxanPatches::conLabel, "integIntactCount: " + std::to_string(integIntactCount));
#endif // ARXAN_DEBUG_INFO
}

void suspendAllOtherThreads() {
    DWORD current_process_id = GetCurrentProcessId();
    DWORD current_thread_id = GetCurrentThreadId();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    THREADENTRY32 te = { 0 };
    te.dwSize = sizeof(te);

    if (Thread32First(snapshot, &te)) {
        do {
            if (te.th32OwnerProcessID == current_process_id && te.th32ThreadID != current_thread_id) {
                HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (thread) {
                    SuspendThread(thread);
                    CloseHandle(thread);
                }
            }
        } while (Thread32Next(snapshot, &te));
    }

    CloseHandle(snapshot);
}

bool o = false;
void ArxanPatches::init() {
#ifdef DEVELOPMENT_BUILD
    Console::initPrint(std::string(__FUNCTION__));
#endif // DEVELOPMENT_BUILD
    createInlineAsmStub();
    //CreateChecksumHealingStub();
    //suspendAllOtherThreads();
}