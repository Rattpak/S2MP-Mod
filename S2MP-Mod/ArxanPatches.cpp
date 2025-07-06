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

uint64_t int2dList[] =
{
    GameUtil::base + 0x74D8, //Arxan_VEH_Return129
    GameUtil::base + 0x7BD5, //Arxan_VEH_Return86
    GameUtil::base + 0x7E5F, //Arxan_VEH_Return61
    GameUtil::base + 0x875A, //Arxan_VEH_Return125
    GameUtil::base + 0x1B5DCE, //Arxan_VEH_Return34
    GameUtil::base + 0x1BD8E5, //Arxan_VEH_Return118
    GameUtil::base + 0x1C8107, //Arxan_VEH_Return4
    //GameUtil::base + 0x1C8DC5, //not really sure
    GameUtil::base + 0x1CB507, //Arxan_VEH_Return70
    GameUtil::base + 0x1CCB90, //Arxan_VEH_Return150
    GameUtil::base + 0x1CD06F, //Arxan_VEH_Return93
    GameUtil::base + 0x1CF4B2, //Arxan_VEH_Return64
    GameUtil::base + 0x1D29C3, //Arxan_VEH_Return121
    GameUtil::base + 0x1D684B, //Arxan_VEH_Return2
    GameUtil::base + 0x1DA539, //Arxan_VEH_Return159
    GameUtil::base + 0x1DAF61, //Arxan_VEH_Return50
    GameUtil::base + 0x1DB341, //Arxan_VEH_Return138
    GameUtil::base + 0x209011, //Arxan_VEH_Return151
    //GameUtil::base + 0x8A275E, //not really sure
    GameUtil::base + 0xB02B48, //Arxan_VEH_Return49
    GameUtil::base + 0x126B5DBE, //Arxan_VEH_Return140
    GameUtil::base + 0x12759961, //Arxan_VEH_Return51
    GameUtil::base + 0x1276640E, //Arxan_VEH_Return84
    GameUtil::base + 0x128892F1, //Arxan_VEH_Return25
    GameUtil::base + 0x12899951, //Arxan_VEH_Return147
    GameUtil::base + 0x128D81CB, //Arxan_VEH_Return3
    GameUtil::base + 0x128E108C, //Arxan_VEH_Return15
    GameUtil::base + 0x128F5C73, //Arxan_VEH_Return108
    GameUtil::base + 0x128F94A1, //Arxan_VEH_Return48
    GameUtil::base + 0x128F9B02, //Arxan_VEH_Return97
    GameUtil::base + 0x12905E51, //Arxan_VEH_Return85
    GameUtil::base + 0x1290C870, //Arxan_VEH_Return90
    GameUtil::base + 0x129E575C, //Arxan_VEH_Return95
    GameUtil::base + 0x129E7387, //Arxan_VEH_Return127
    GameUtil::base + 0x12A22AF0, //Arxan_VEH_Return123
    GameUtil::base + 0x12A2416F, //Arxan_VEH_Return137
    GameUtil::base + 0x12A7A188, //Arxan_VEH_Return153
    GameUtil::base + 0x12A94A90, //Arxan_VEH_Return145
    GameUtil::base + 0x12A95CA5, //Arxan_VEH_Return113
    GameUtil::base + 0x12AC27FC, //Arxan_VEH_Return156
    GameUtil::base + 0x12AC2BBF, //Arxan_VEH_Return88
    GameUtil::base + 0x12ADA6F5, //Arxan_VEH_Return53
    GameUtil::base + 0x12ADB712, //Arxan_VEH_Return133
    GameUtil::base + 0x12B03C59, //Arxan_VEH_Return63
    GameUtil::base + 0x12CD0A01, //Arxan_VEH_Return134
    GameUtil::base + 0x12CD8A98, //Arxan_VEH_Return119
    GameUtil::base + 0x12D1DD70, //Arxan_VEH_Return46
    GameUtil::base + 0x12DC9D55, //Arxan_VEH_Return149
    //GameUtil::base + 0x12F25461, //not really sure
    GameUtil::base + 0x12F32CA4, //Arxan_VEH_Return152
    GameUtil::base + 0x12F3C835, //Arxan_VEH_Return56
    GameUtil::base + 0x12F70CDF, //Arxan_VEH_Return94
    GameUtil::base + 0x12F72269, //Arxan_VEH_Return83
    GameUtil::base + 0x12FAAB31, //Arxan_VEH_Return99
    GameUtil::base + 0x12FAC920, //Arxan_VEH_Return13
    GameUtil::base + 0x12FB0F2E, //Arxan_VEH_Return45
    GameUtil::base + 0x130068D7, //Arxan_VEH_Return55
    GameUtil::base + 0x1304891F, //Arxan_VEH_Return37
    GameUtil::base + 0x13054E04, //Arxan_VEH_Return101
    GameUtil::base + 0x130781CC, //Arxan_VEH_Return87
    GameUtil::base + 0x13078464, //Arxan_VEH_Return72
    GameUtil::base + 0x131487D5, //Arxan_VEH_Return60
    GameUtil::base + 0x1317049C, //Arxan_VEH_Return47
    GameUtil::base + 0x131B977F, //Arxan_VEH_Return33
    GameUtil::base + 0x132A3F58, //Arxan_VEH_Return52
    GameUtil::base + 0x132AA8A9, //Arxan_VEH_Return112
    GameUtil::base + 0x132BF741, //Arxan_VEH_Return32
    GameUtil::base + 0x132EFB78, //Arxan_VEH_Return67
    GameUtil::base + 0x132FCF53, //Arxan_VEH_Return68
    GameUtil::base + 0x132FD4DD, //Arxan_VEH_Return116
};

std::vector<uint64_t> integrityIntact =
{
    0x125F106D,
    0x126113DB,
    0x1261E7F7,
    0x1262C7D3,
    0x12636FD7,
    0x1265AD33,
    0x1268CCAB,
    0x1268F33C,
    0x12694BF5,
    0x126BC85C,
    0x1272592F,
    0x12725FC1,
    0x127673A5,
    0x12767987,
    0x1276DF35,
    0x1286BE95,
    0x1286F66E,
    0x12876BB5,
    0x128B2811,
    0x128B3B71,
    0x128B81B7,
    0x128D537A,
    0x128D78B4,
    0x128EA789,
    0x128F34AC,
    0x12A22834,
    0x12A27D34,
    0x12A5E1B6,
    0x12A5F5FE,
    0x12A8C13A,
    0x12B5D77D,
    0x12D0D6DC,
    0x12D1A6A8,
    0x12D1F988,
    0x12D716C7,
    0x12D719CD,
    0x12DAA4C1,
    0x12DAED8C,
    0x12DBC4E4,
    0x12DC09D6,
    0x12DF7B99,
    0x12E9C69F,
    0x12F12CAF,
    0x12F1EFDC,
    0x12F2D54B,
    0x12F6A9A1,
    0x12FE21D7,
    0x1300A699,
    0x1300DA14,
    0x1300FE24,
    0x130AE017,
    0x13143663,
    0x13145DBF,
    0x1314EE37,
    0x13167968,
    0x131BA183,
    0x131BEA78,
    0x131C061D,
    0x132B25D6,
};

std::vector<uint64_t> integrityIntactBig =
{
    0x125E8499,
    0x125F4C2C,
    0x1260F296,
    0x1261080C,
    0x126132B6,
    0x1261CB24,
    0x1264911E,
    0x1268A61F,
    0x126985C0,
    0x12760FB4,
    0x12776ABB,
    0x128746D7,
    0x12875E85,
    0x128829ED,
    0x128DC2EA,
    0x12909BFA,
    0x1290E4E2,
    0x129B1EB6,
    0x129BC9E4,
    0x129E67F7,
    0x129EF8B6,
    0x129F03B1,
    0x129F881E,
    0x12A12FAE,
    0x12A14033,
    0x12A5EFDF,
    0x12A6F114,
    0x12A88D14,
    0x12A8C8D3,
    0x12A92C01,
    0x12AC060D,
    0x12AC0BBB,
    0x12B05D9E,
    0x12F13B1C,
    0x12F2E7A7,
    0x12F2EAFB,
    0x12F31FA2,
    0x12F6C0EB,
    0x12F726D1,
    0x12F86C61,
    0x12FAF49D,
    0x12FB0FCD,
    0x130077CF,
    0x13053791,
    0x1307A31D,
    0x1313A3EF,
    0x131498F4,
    0x13168895,
    0x1316B43B,
    0x1316B8A3,
    0x1316DDE3,
    0x13181975,
    0x13183303,
    0x131B04BE,
    0x132A8331,
    0x132B5571,
    0x132BC3F4,
    0x132EFF25,
    0x132F4C16,
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


bool is_relatively_far(const void* pointer, const void* data) {
    const int64_t diff = size_t(data) - (size_t(pointer) + 5);
    const auto small_diff = int32_t(diff);
    return diff != int64_t(small_diff);
}

uint8_t* allocate_somewhere_near(const void* base_address, const size_t size) {
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

            //if (memcmp(intactchecksumHooks[i].functionAddress, intactchecksumHooks[i].buffer, sizeof(uint8_t) * 7))
            //{
            //    uint64_t idaAddress = (uint64_t)intactchecksumHooks[i].functionAddress - baseAddressStart + StartOfBinary;
            //
            //    printf("%llx %llx got changed\n", idaAddress, (uint64_t)intactchecksumHooks[i].functionAddress);
            //    fprintf(logFile, "%llx got changed\n", idaAddress);
            //    fflush(logFile);
            //}

            VirtualProtect(intactchecksumHooks[i].functionAddress, sizeof(uint8_t) * 7, PAGE_EXECUTE_READWRITE, &old_protect);
            memcpy(intactchecksumHooks[i].functionAddress, intactchecksumHooks[i].buffer, sizeof(uint8_t) * 7);
            VirtualProtect(intactchecksumHooks[i].functionAddress, sizeof(uint8_t) * 7, old_protect, &old_protect);
            FlushInstructionCache(GetCurrentProcess(), intactchecksumHooks[i].functionAddress, sizeof(uint8_t) * 7);
        }

        for (int i = 0; i < intactBigchecksumHooks.size(); i++)
        {
            DWORD old_protect{};

            //if (memcmp(intactBigchecksumHooks[i].functionAddress, intactBigchecksumHooks[i].buffer, sizeof(uint8_t) * 7))
            //{
            //    uint64_t idaAddress = (uint64_t)intactBigchecksumHooks[i].functionAddress - baseAddressStart + StartOfBinary;
            //
            //    printf("%llx %llx got changed\n", idaAddress, (uint64_t)intactBigchecksumHooks[i].functionAddress);
            //    fprintf(logFile, "%llx got changed\n", idaAddress);
            //    fflush(logFile);
            //}

            VirtualProtect(intactBigchecksumHooks[i].functionAddress, sizeof(uint8_t) * 10, PAGE_EXECUTE_READWRITE, &old_protect);
            memcpy(intactBigchecksumHooks[i].functionAddress, intactBigchecksumHooks[i].buffer, sizeof(uint8_t) * 10);
            VirtualProtect(intactBigchecksumHooks[i].functionAddress, sizeof(uint8_t) * 10, old_protect, &old_protect);
            FlushInstructionCache(GetCurrentProcess(), intactBigchecksumHooks[i].functionAddress, sizeof(uint8_t) * 10);
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


void createInlineAsmStub() {
    int integIntactCount = integrityIntact.size();
    int integIntactBigCount = integrityIntactBig.size();
    int totalIntegrityCount = integIntactCount + integIntactBigCount;

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

    LPVOID asmBigStubLocation = allocate_somewhere_near(GetModuleHandle(nullptr), allocationSize * 0x80);
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


        // create assembly stub content
        // TODO: we could create three different asmjit build sections for each type
        // so we don't have if statements inbetween instructions for the cost of LOC but it would be more readible
        static asmjit::JitRuntime runtime;
        asmjit::CodeHolder code;
        code.init(runtime.environment());
        asmjit::x86::Assembler a(&code);

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

            a.mov(asmjit::x86::r11, addressToJump);	// r11 is being used but should be fine based on documentation

            a.push(asmjit::x86::r11);
        }

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

        previousStubOffset = currentStubOffset + sizeof(uint8_t) * code.codeSize() + 0x8;
    }

#ifdef DEVELOPMENT_BUILD
    Console::labelPrint(ArxanPatches::conLabel ,std::string(__FUNCTION__) + " complete");
#endif // DEVELOPMENT_BUILD
}

void ArxanPatches::init() {
#ifdef DEVELOPMENT_BUILD
    Console::initPrint(std::string(__FUNCTION__));
#endif // DEVELOPMENT_BUILD
    createInlineAsmStub();

}