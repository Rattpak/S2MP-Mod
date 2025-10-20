/////////////////////////////
//        Hook
//	x64 Hooking Util
/////////////////////////////
#include "pch.h"
#include "Hook.hpp"
#include <cstdint>

#include "memory.h"

//Credit:
//https://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html

void* Hook::allocatePageNearAddress(void* targetAddr) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    const uint64_t PAGE_SIZE = sysInfo.dwPageSize;

    uint64_t startAddr = (uint64_t(targetAddr) & ~(PAGE_SIZE - 1)); //round down to nearest page boundary
    uint64_t minAddr = std::min(startAddr - 0x7FFFFF00, (uint64_t)sysInfo.lpMinimumApplicationAddress);
    uint64_t maxAddr = std::max(startAddr + 0x7FFFFF00, (uint64_t)sysInfo.lpMaximumApplicationAddress);

    uint64_t startPage = (startAddr - (startAddr % PAGE_SIZE));

    uint64_t pageOffset = 1;
    while (1) {
        uint64_t byteOffset = pageOffset * PAGE_SIZE;
        uint64_t highAddr = startPage + byteOffset;
        uint64_t lowAddr = (startPage > byteOffset) ? startPage - byteOffset : 0;

        bool needsExit = highAddr > maxAddr && lowAddr < minAddr;

        if (highAddr < maxAddr) {
            void* outAddr = VirtualAlloc((void*)highAddr, PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (outAddr)
                return outAddr;
        }

        if (lowAddr > minAddr) {
            void* outAddr = VirtualAlloc((void*)lowAddr, PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (outAddr != nullptr)
                return outAddr;
        }

        pageOffset++;

        if (needsExit) {
            break;
        }
    }

    return nullptr;
}

void Hook::writeAbsoluteJump64(void* absJumpMemory, void* addrToJumpTo) {
    uint8_t absJumpInstructions[] = {
      0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x41, 0xFF, 0xE2
    };

    uint64_t addrToJumpTo64 = (uint64_t)addrToJumpTo;
    memcpy(&absJumpInstructions[2], &addrToJumpTo64, sizeof(addrToJumpTo64));
    memcpy(absJumpMemory, absJumpInstructions, sizeof(absJumpInstructions));
}

void Hook::installHook(void* func2hook, void* payloadFunction) {
    void* relayFuncMemory = allocatePageNearAddress(func2hook);
    writeAbsoluteJump64(relayFuncMemory, payloadFunction);

    DWORD oldProtect;
    VirtualProtect(func2hook, 1024, PAGE_EXECUTE_READWRITE, &oldProtect);

    uint8_t jmpInstruction[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };

    const uint64_t relAddr = (uint64_t)relayFuncMemory - ((uint64_t)func2hook + sizeof(jmpInstruction));
    memcpy(jmpInstruction + 1, &relAddr, 4);
    memcpy(func2hook, jmpInstruction, sizeof(jmpInstruction));
}

uint8_t* allocate_somewhere_near(const void* base_address, const size_t size)
{
	size_t offset = 0;
	while (true)
	{
		offset += size;
		auto* target_address = static_cast<const uint8_t*>(base_address) - offset;
		if (utils::hook::is_relatively_far(base_address, target_address))
		{
			return nullptr;
		}

		const auto res = VirtualAlloc(const_cast<uint8_t*>(target_address), size, MEM_RESERVE | MEM_COMMIT,
			PAGE_EXECUTE_READWRITE);
		if (res)
		{
			if (utils::hook::is_relatively_far(base_address, target_address))
			{
				VirtualFree(res, 0, MEM_RELEASE);
				return nullptr;
			}

			return static_cast<uint8_t*>(res);
		}
	}
}

class memory
{
public:
	memory() = default;

	memory(const void* ptr)
		: memory()
	{
		this->length_ = 0x1000;
		this->buffer_ = allocate_somewhere_near(ptr, this->length_);
		if (!this->buffer_)
		{
			throw std::runtime_error("Failed to allocate");
		}
	}

	~memory()
	{
		if (this->buffer_)
		{
			VirtualFree(this->buffer_, 0, MEM_RELEASE);
		}
	}

	memory(memory&& obj) noexcept
		: memory()
	{
		this->operator=(std::move(obj));
	}

	memory& operator=(memory&& obj) noexcept
	{
		if (this != &obj)
		{
			this->~memory();
			this->buffer_ = obj.buffer_;
			this->length_ = obj.length_;
			this->offset_ = obj.offset_;

			obj.buffer_ = nullptr;
			obj.length_ = 0;
			obj.offset_ = 0;
		}

		return *this;
	}

	void* allocate(const size_t length)
	{
		if (!this->buffer_)
		{
			return nullptr;
		}

		if (this->offset_ + length > this->length_)
		{
			return nullptr;
		}

		const auto ptr = this->get_ptr();
		this->offset_ += length;
		return ptr;
	}

	void* get_ptr() const
	{
		return this->buffer_ + this->offset_;
	}

private:
	uint8_t* buffer_{};
	size_t length_{};
	size_t offset_{};
};

void* initialize_min_hook()
{
	static class min_hook_init
	{
	public:
		min_hook_init()
		{
			if (MH_Initialize() != MH_OK)
			{
				throw std::runtime_error("Failed to initialize MinHook");
			}
		}

		~min_hook_init()
		{
			MH_Uninitialize();
		}
	} min_hook_init;
	return &min_hook_init;
}

utils::hook::detour::detour()
{
	(void)initialize_min_hook();
}

utils::hook::detour::detour(const size_t place, void* target)
	: detour(reinterpret_cast<void*>(place), target)
{
}

utils::hook::detour::detour(void* place, void* target)
	: detour()
{
	this->create(place, target);
}

utils::hook::detour::~detour()
{
	this->clear();
}

void utils::hook::detour::queue_enable()
{
	MH_QueueEnableHook(this->place_);
}

void utils::hook::detour::queue_disable()
{
	MH_QueueDisableHook(this->place_);
}

void utils::hook::detour::enable()
{
	MH_EnableHook(this->place_);

	if (!this->moved_data_.empty())
	{
		this->move();
	}
}

void utils::hook::detour::disable()
{
	this->un_move();
	MH_DisableHook(this->place_);
}

void utils::hook::detour::create(void* place, void* target)
{
	this->clear();
	this->place_ = place;

	if (MH_CreateHook(this->place_, target, &this->original_) != MH_OK)
	{
		//throw std::runtime_error(string::va("Unable to create hook at location: %p", this->place_));
	}

	this->enable();
}

void utils::hook::detour::create(const size_t place, void* target)
{
	this->create(reinterpret_cast<void*>(place), target);
}

void utils::hook::detour::clear()
{
	if (this->place_)
	{
		this->un_move();
		MH_RemoveHook(this->place_);
	}

	this->place_ = nullptr;
	this->original_ = nullptr;
	this->moved_data_ = {};
}

void* utils::hook::detour::follow_branch(void* address)
{
	auto* const data = static_cast<uint8_t*>(address);
	if (*data != 0xE8 && *data != 0xE9)
	{
		throw std::runtime_error("No branch instruction found");
	}

	return extract<void*>(data + 1);
}

std::vector<uint8_t> utils::hook::detour::move_hook(void* pointer)
{
	std::vector<uint8_t> original_data{};

	auto* data_ptr = static_cast<uint8_t*>(pointer);
	if (data_ptr[0] == 0xE9)
	{
		original_data.resize(6);
		memmove(original_data.data(), pointer, original_data.size());

		auto* target = follow_branch(data_ptr);
		utils::hook::nop(data_ptr, 1);
		utils::hook::jump(data_ptr + 1, target);
	}
	else if (data_ptr[0] == 0xFF && data_ptr[1] == 0x25)
	{
		original_data.resize(15);
		memmove(original_data.data(), pointer, original_data.size());

		utils::hook::copy(data_ptr + 1, data_ptr, 14);
		utils::hook::nop(data_ptr, 1);
	}
	else
	{
		throw std::runtime_error("No branch instruction found");
	}

	return original_data;
}

std::vector<uint8_t> utils::hook::detour::move_hook(const size_t pointer)
{
	return move_hook(reinterpret_cast<void*>(pointer));
}

void utils::hook::detour::move()
{
	this->moved_data_ = move_hook(this->place_);
}

void* utils::hook::detour::get_place() const
{
	return this->place_;
}

void* utils::hook::detour::get_original() const
{
	return this->original_;
}

void utils::hook::detour::un_move()
{
	if (!this->moved_data_.empty())
	{
		utils::hook::copy(this->place_, this->moved_data_.data(), this->moved_data_.size());
	}
}

void* get_memory_near(const void* address, const size_t size)
{
	static utils::concurrency::container<std::vector<memory>> memory_container{};

	return memory_container.access<void*>([&](std::vector<memory>& memories)
		{
			for (auto& memory : memories)
			{
				if (!utils::hook::is_relatively_far(address, memory.get_ptr()))
				{
					const auto buffer = memory.allocate(size);
					if (buffer)
					{
						return buffer;
					}
				}
			}

			memories.emplace_back(address);
			return memories.back().allocate(size);
		});
}

void utils::hook::nop(void* place, const size_t length)
{
	DWORD old_protect{};
	VirtualProtect(place, length, PAGE_EXECUTE_READWRITE, &old_protect);

	std::memset(place, 0x90, length);

	VirtualProtect(place, length, old_protect, &old_protect);
	FlushInstructionCache(GetCurrentProcess(), place, length);
}

void utils::hook::nop(const size_t place, const size_t length)
{
	nop(reinterpret_cast<void*>(place), length);
}

void utils::hook::copy(void* place, const void* data, const size_t length)
{
	DWORD old_protect{};
	VirtualProtect(place, length, PAGE_EXECUTE_READWRITE, &old_protect);

	std::memmove(place, data, length);

	VirtualProtect(place, length, old_protect, &old_protect);
	FlushInstructionCache(GetCurrentProcess(), place, length);
}

void utils::hook::copy(const size_t place, const void* data, const size_t length)
{
	copy(reinterpret_cast<void*>(place), data, length);
}

void utils::hook::copy_string(void* place, const char* str)
{
	copy(reinterpret_cast<void*>(place), str, strlen(str) + 1);
}

void utils::hook::copy_string(const size_t place, const char* str)
{
	copy_string(reinterpret_cast<void*>(place), str);
}

bool utils::hook::is_relatively_far(const void* pointer, const void* data, const int offset)
{
	const int64_t diff = size_t(data) - (size_t(pointer) + offset);
	const auto small_diff = int32_t(diff);
	return diff != int64_t(small_diff);
}

void utils::hook::call(void* pointer, void* data)
{
	if (is_relatively_far(pointer, data))
	{
		auto* trampoline = get_memory_near(pointer, 14);
		if (!trampoline)
		{
			throw std::runtime_error("Too far away to create 32bit relative branch");
		}

		call(pointer, trampoline);
		jump(trampoline, data, true, true);
		return;
	}

	uint8_t copy_data[5];
	copy_data[0] = 0xE8;
	*reinterpret_cast<int32_t*>(&copy_data[1]) = int32_t(size_t(data) - (size_t(pointer) + 5));

	auto* patch_pointer = PBYTE(pointer);
	copy(patch_pointer, copy_data, sizeof(copy_data));
}

void utils::hook::call(const size_t pointer, void* data)
{
	return call(reinterpret_cast<void*>(pointer), data);
}

void utils::hook::call(const size_t pointer, const size_t data)
{
	return call(pointer, reinterpret_cast<void*>(data));
}

void utils::hook::jump(void* pointer, void* data, const bool use_far, const bool use_safe)
{
	static const unsigned char jump_data[] = {
		0x48, 0xb8, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0xff, 0xe0
	};

	static const unsigned char jump_data_safe[] = {
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00
	};

	if (!use_far && is_relatively_far(pointer, data))
	{
		auto* trampoline = get_memory_near(pointer, 14);
		if (!trampoline)
		{
			throw std::runtime_error("Too far away to create 32bit relative branch");
		}
		jump(pointer, trampoline, false, false);
		jump(trampoline, data, true, true);
		return;
	}

	auto* patch_pointer = PBYTE(pointer);

	if (use_far)
	{
		if (use_safe)
		{
			uint8_t copy_data[sizeof(jump_data_safe) + sizeof(data)];
			memcpy(copy_data, jump_data_safe, sizeof(jump_data_safe));
			memcpy(copy_data + sizeof(jump_data_safe), &data, sizeof(data));

			copy(patch_pointer, copy_data, sizeof(copy_data));
		}
		else
		{
			uint8_t copy_data[sizeof(jump_data)];
			memcpy(copy_data, jump_data, sizeof(jump_data));
			memcpy(copy_data + 2, &data, sizeof(data));

			copy(patch_pointer, copy_data, sizeof(copy_data));
		}
	}
	else
	{
		uint8_t copy_data[5];
		copy_data[0] = 0xE9;
		*reinterpret_cast<int32_t*>(&copy_data[1]) = int32_t(size_t(data) - (size_t(pointer) + 5));

		copy(patch_pointer, copy_data, sizeof(copy_data));
	}
}

void utils::hook::jump(const size_t pointer, void* data, const bool use_far, const bool use_safe)
{
	return jump(reinterpret_cast<void*>(pointer), data, use_far, use_safe);
}

void utils::hook::jump(const size_t pointer, const size_t data, const bool use_far, const bool use_safe)
{
	return jump(pointer, reinterpret_cast<void*>(data), use_far, use_safe);
}