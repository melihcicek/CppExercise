#ifndef ALLOCATOR_H
#define ALLOCATOR_H


namespace mc {
	template<typename T, size_t N = 1024>
	class Allocator {
		char m_buffer[sizeof(T) * N];

	public:
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using value_type = T;

		template <typename U>
		struct rebind
		{
			typedef Allocator<U, N> other;
		};

		Allocator() = default;

		pointer allocate(size_t)
		{
			return reinterpret_cast<T*>(m_buffer);
		}

		void deallocate(pointer, std::size_t) {	}

		template<typename T, typename ...Args>
		void construct(T* t, Args ...args)
		{
			new (t) T(std::forward<Args>(args)...);
		}

		void destroy(pointer t)
		{
			if (std::is_destructible_v<T>)
				t->~T();
		}

		static size_t size()
		{
			return sizeof(m_buffer) / sizeof(T);
		}
	};
}

#endif
