#pragma once

#include <cstdint>

template <typename T>
class symbol
{
public:
	symbol(const size_t address)
		: address_(reinterpret_cast<T*>(address))
	{
	}

	T* get() const
	{
		return reinterpret_cast<T*>(reinterpret_cast<uint64_t>(address_));
	}

	operator T* () const
	{
		return this->get();
	}

	T* operator->() const
	{
		return this->get();
	}

private:
	T* address_;
};

extern uintptr_t base;
size_t _b(size_t val);
size_t operator"" _b(size_t val);