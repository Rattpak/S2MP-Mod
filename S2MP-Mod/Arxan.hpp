#pragma once
#include "structs.h"
#include <string>
#include "Console.hpp"
#include "GameUtil.hpp"
#include <MinHook.h>
#include <Windows.h>
#include <array>
#include <mutex>
#include <winternl.h>
#include <TlHelp32.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include <intrin.h>
#include <ctime>
#include <Psapi.h>
#include <iomanip>
#include <sstream>
#include <thread>
#include <atomic>
#include <winternl.h>
#include <ntstatus.h>
#include <asmjit/asmjit.h>
#include <unordered_map>
#include "DevDef.h"

struct intactChecksumHook
{
	uint64_t* functionAddress;
	uint8_t buffer[7];
};

struct intactBigChecksumHook
{
	uint64_t* functionAddress;
	uint8_t buffer[7 + 3];
};

enum checksumType {
	intactSmall,
	intactBig,
	//split //not in the gamepass build
};

struct inlineAsmStub {
	void* functionAddress;
	uint8_t* buffer;
	size_t bufferSize;
	checksumType type;
};


#define pushad64() a.push(asmjit::x86::rax); 	\
				a.push(asmjit::x86::rcx); 	\
				a.push(asmjit::x86::rdx);	\
				a.push(asmjit::x86::rbx);	\
				a.push(asmjit::x86::rsp);	\
				a.push(asmjit::x86::rbp);	\
				a.push(asmjit::x86::rsi);	\
				a.push(asmjit::x86::rdi);	\
				a.push(asmjit::x86::r8);	\
				a.push(asmjit::x86::r9);	\
				a.push(asmjit::x86::r10);	\
				a.push(asmjit::x86::r11);	\
				a.push(asmjit::x86::r12);	\
				a.push(asmjit::x86::r13);	\
				a.push(asmjit::x86::r14);	\
				a.push(asmjit::x86::r15);

#define popad64() a.pop(asmjit::x86::r15); 	\
				a.pop(asmjit::x86::r14);	\
				a.pop(asmjit::x86::r13);	\
				a.pop(asmjit::x86::r12);	\
				a.pop(asmjit::x86::r11);	\
				a.pop(asmjit::x86::r10);	\
				a.pop(asmjit::x86::r9);		\
				a.pop(asmjit::x86::r8);		\
				a.pop(asmjit::x86::rdi);	\
				a.pop(asmjit::x86::rsi);	\
				a.pop(asmjit::x86::rbp);	\
				a.pop(asmjit::x86::rsp);	\
				a.pop(asmjit::x86::rbx);	\
				a.pop(asmjit::x86::rdx);	\
				a.pop(asmjit::x86::rcx);	\
				a.pop(asmjit::x86::rax);

#define popad64WithoutRAX() a.pop(asmjit::x86::r11);	\
				a.pop(asmjit::x86::r10);	\
				a.pop(asmjit::x86::r9);		\
				a.pop(asmjit::x86::r8);		\
				a.pop(asmjit::x86::rdi);	\
				a.pop(asmjit::x86::rsi);	\
				a.pop(asmjit::x86::rbp);	\
				a.pop(asmjit::x86::rsp);	\
				a.pop(asmjit::x86::rbx);	\
				a.pop(asmjit::x86::rdx);	\
				a.pop(asmjit::x86::rcx);

#define pushad64_Min() a.push(asmjit::x86::rax); 	\
				a.push(asmjit::x86::rcx); 	\
				a.push(asmjit::x86::rdx);	\
				a.push(asmjit::x86::rbx);	\
				a.push(asmjit::x86::rsp);	\
				a.push(asmjit::x86::rbp);	\
				a.push(asmjit::x86::rsi);	\
				a.push(asmjit::x86::rdi);	\
				a.push(asmjit::x86::r8);	\
				a.push(asmjit::x86::r9);	\
				a.push(asmjit::x86::r10);	\
				a.push(asmjit::x86::r11);


#define popad64_Min() a.pop(asmjit::x86::r11);	\
				a.pop(asmjit::x86::r10);	\
				a.pop(asmjit::x86::r9);		\
				a.pop(asmjit::x86::r8);		\
				a.pop(asmjit::x86::rdi);	\
				a.pop(asmjit::x86::rsi);	\
				a.pop(asmjit::x86::rbp);	\
				a.pop(asmjit::x86::rsp);	\
				a.pop(asmjit::x86::rbx);	\
				a.pop(asmjit::x86::rdx);	\
				a.pop(asmjit::x86::rcx);	\
				a.pop(asmjit::x86::rax);

class DebugPatches {
public:
	static void init();
	static uintptr_t base;
	static uintptr_t rawBase;

	static std::string conLabel;
};

class ArxanPatches {
public:
	static void init();

	static std::string conLabel;
};