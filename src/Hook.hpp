#pragma once
class Hook {
public:
	static void installHook(void* func2hook, void* payloadFunction);
private:
	static void* allocatePageNearAddress(void* targetAddr);
	static void writeAbsoluteJump64(void* absJumpMemory, void* addrToJumpTo);
};