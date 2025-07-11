#pragma once

#include <asmjit/core/jitruntime.h>
#include <asmjit/x86/x86assembler.h>

class Hook {
public:
	static void installHook(void* func2hook, void* payloadFunction);
private:
	static void* allocatePageNearAddress(void* targetAddr);
	static void writeAbsoluteJump64(void* absJumpMemory, void* addrToJumpTo);
};

#include <mutex>
namespace utils
{
	namespace concurrency
	{
		template <typename T, typename MutexType = std::mutex>
		class container
		{
		public:
			template <typename R = void, typename F>
			R access(F&& accessor) const
			{
				std::lock_guard<MutexType> _{ mutex_ };
				return accessor(object_);
			}

			template <typename R = void, typename F>
			R access(F&& accessor)
			{
				std::lock_guard<MutexType> _{ mutex_ };
				return accessor(object_);
			}

			template <typename R = void, typename F>
			R access_with_lock(F&& accessor) const
			{
				std::unique_lock<MutexType> lock{ mutex_ };
				return accessor(object_, lock);
			}

			template <typename R = void, typename F>
			R access_with_lock(F&& accessor)
			{
				std::unique_lock<MutexType> lock{ mutex_ };
				return accessor(object_, lock);
			}

			T& get_raw() { return object_; }
			const T& get_raw() const { return object_; }

		private:
			mutable MutexType mutex_{};
			T object_{};
		};
	}

	class hook
	{
	public:
		class detour
		{
		public:
			detour();
			detour(void* place, void* target);
			detour(size_t place, void* target);
			~detour();

			detour(detour&& other) noexcept
			{
				this->operator=(std::move(other));
			}

			detour& operator=(detour&& other) noexcept
			{
				if (this != &other)
				{
					this->clear();

					this->place_ = other.place_;
					this->original_ = other.original_;
					this->moved_data_ = other.moved_data_;

					other.place_ = nullptr;
					other.original_ = nullptr;
					other.moved_data_ = {};
				}

				return *this;
			}

			detour(const detour&) = delete;
			detour& operator=(const detour&) = delete;

			void enable();
			void disable();

			void queue_enable();
			void queue_disable();

			void create(void* place, void* target);
			void create(size_t place, void* target);
			void clear();

			void* follow_branch(void* address);

			void move();

			std::vector<uint8_t> move_hook(void* pointer);
			std::vector<uint8_t> move_hook(size_t pointer);

			void* get_place() const;

			template <typename T>
			T extract(void* address)
			{
				auto* const data = static_cast<uint8_t*>(address);
				const auto offset = *reinterpret_cast<int32_t*>(data);
				return reinterpret_cast<T>(data + offset + 4);
			}

			template <typename T>
			T* get() const
			{
				return static_cast<T*>(this->get_original());
			}

			template <typename T = void, typename... Args>
			T invoke(Args ... args)
			{
				return static_cast<T(*)(Args ...)>(this->get_original())(args...);
			}

			[[nodiscard]] void* get_original() const;

		private:
			std::vector<uint8_t> moved_data_{};
			void* place_{};
			void* original_{};

			void un_move();
		};

		static void nop(void* place, size_t length);
		static void nop(size_t place, size_t length);

		static void copy(void* place, const void* data, size_t length);
		static void copy(size_t place, const void* data, size_t length);

		static void copy_string(void* place, const char* str);
		static void copy_string(size_t place, const char* str);

		static bool is_relatively_far(const void* pointer, const void* data, int offset = 5);

		static void call(void* pointer, void* data);
		static void call(size_t pointer, void* data);
		static void call(size_t pointer, size_t data);

		static void jump(void* pointer, void* data, bool use_far = false, bool use_safe = false);
		static void jump(size_t pointer, void* data, bool use_far = false, bool use_safe = false);
		static void jump(size_t pointer, size_t data, bool use_far = false, bool use_safe = false);
	};
};