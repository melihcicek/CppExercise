#ifndef VECTOR_H
#define VECTOR_H

#include <type_traits>
#include <typeinfo>
#include "Allocator.h"
#include <memory>

namespace mc {
#define DEFAULT_SIZE	8
#define INC_RATE		2

	template<typename T, typename Alloc = std::allocator<T>>
	class Vector {
		using value_type = T;
		Alloc m_alloc;
		T* m_buffer;
		size_t m_capacity = 1;
		size_t m_size = 0;

		void inc_capacity()
		{
			m_capacity *= INC_RATE;
			if (std::is_same_v<decltype(m_alloc), mc::Allocator<T> >) {
				if (m_capacity >= mc::Allocator<T>::size()) {
					throw std::bad_alloc{};
				}
			}
			else {
				T* temp;
				temp = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity);
				std::move(m_buffer, m_buffer + m_size, temp);
				std::allocator_traits<Alloc>::destroy(m_alloc, m_buffer);
				m_buffer = temp;
			}
		}

		void inc_capacity(size_t capacity)
		{
			T* temp;
			m_capacity = capacity;

			if (m_capacity > m_alloc.get_size())
				throw std::bad_alloc{};
		}
		void create_init_buffer(const std::initializer_list<T>& ilist)
		{
			m_size = ilist.size();
			m_capacity = m_size * INC_RATE;
			m_buffer = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity);
			std::move(ilist.begin(), ilist.end(), m_buffer);
		}

		void deep_copy(T* src, size_t len, T* dest)
		{
			for (size_t i = 0; i < len; ++i) {
				dest[i] = src[i];
			}
		}

		class Iterator;
		void destroy_element(Iterator it, std::ptrdiff_t diff)
		{
			for (auto i = 0; i < diff; i++) {
				if (std::is_destructible_v<value_type>) {
					std::allocator_traits<Alloc>::destroy(m_alloc, &(*it));
				}

				it++;
			}
		}
		void destroy_element(Iterator begin, Iterator end)
		{
			for (; begin != end; ++begin) {
				if (std::is_destructible_v<value_type>) {
					std::allocator_traits<Alloc>::destroy(m_alloc, &(*begin));
				}
			}
		}
		void destroy_element(Iterator it)
		{
			std::allocator_traits<Alloc>::destroy(m_alloc, &(*it));
		}

	public:
		Vector() : m_capacity{ DEFAULT_SIZE }
		{
			m_buffer = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity);
			if (std::is_same_v<decltype(m_alloc), mc::Allocator<T> >)
				m_capacity = mc::Allocator<T>::size();
		}

		Vector(size_t capacity) : m_capacity{ capacity }
		{
			m_buffer = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity);
		}

		Vector(const std::initializer_list<T>& ilist)
		{
			create_init_buffer(ilist);
		}

		Vector(std::initializer_list<T>&& ilist)
		{
			create_init_buffer(ilist);
		}

		Vector(Vector&& other) noexcept
			: m_capacity(other.capacity()), m_size(other.size()), m_buffer(other.m_buffer)
		{
			other.m_size = 0;
			other.m_capacity = 0;
			other.m_buffer = nullptr;
		}

		~Vector()
		{
			destroy_element(begin(), end());
			std::allocator_traits<Alloc>::deallocate(m_alloc, m_buffer, m_capacity);
			m_size = 0;
			m_capacity = 0;
		}

		void push_back(const T& val)
		{
			if (m_size == m_capacity) {
				inc_capacity();
			}
			m_buffer[m_size++] = val;
		}

		template<typename... Args>
		void emplace_back(Args&&... args)
		{
			if (m_size == m_capacity) {
				inc_capacity();
			}

			std::allocator_traits<Alloc>::construct(m_alloc, m_buffer + m_size, std::forward<Args>(args)...);
			++m_size;

		}

		void push_back(T&& val)
		{
			emplace_back(std::move(val));
		}

		T& front() const
		{
			return m_buffer[0];
		}

		T& back() const
		{
			return m_buffer[m_size - 1];
		}

		T& pop_back()
		{
			if (m_size != 0)
				return m_buffer[--m_size];

			throw std::range_error("Index out of memory");
		}

		size_t size() const
		{
			return m_size;
		}

		size_t capacity() const
		{
			return m_capacity;
		}

		bool empty() const
		{
			return m_size == 0;
		}

		void clear()
		{
			m_size = 0;
		}

		T& operator[](size_t i)
		{
			return m_buffer[i];
		}

		Vector& operator=(Vector&& other) noexcept
		{
			if (this == &other)
				return *this;

			if (m_buffer)
				delete[] m_buffer;

			m_buffer = other.m_buffer;
			m_size = other.size();
			m_capacity = other.capacity();
			other.m_buffer = nullptr;
			return *this;
		}

		Vector& operator=(const Vector& other)
		{
			if (this == &other)
				return *this;

			if (m_buffer)
				delete[] m_buffer;

			m_size = other.size();
			m_capacity = other.capacity();

			m_buffer = new T[m_capacity];
			deep_copy(other.m_buffer, other.size(), m_buffer);

			return *this;
		}

		class Iterator {
			friend class Vector<T, Alloc>;

		protected:
			T* m_iter;
			size_t m_idx;
		private:
			Iterator(T* it, size_t idx) : m_iter{ it }, m_idx{ idx } {}

		public:
			T& operator*()
			{
				return m_iter[m_idx];
			}

			Iterator& operator++()
			{
				m_idx++;
				return *this;
			}

			Iterator operator++(int)
			{
				Iterator temp = *this;
				m_idx++;
				return temp;
			}

			Iterator& operator--()
			{
				m_idx--;
				return *this;
			}

			Iterator operator--(int)
			{
				Iterator temp = *this;
				m_idx--;
				return temp;
			}

			Iterator operator+(size_t i)
			{
				m_idx += i;
				return *this;
			}

			Iterator operator-(size_t i)
			{
				m_idx -= i;
				return *this;
			}

			std::ptrdiff_t operator-(Iterator it) const
			{
				return m_idx - it.m_idx;
			}

			bool operator==(const Iterator& r)
			{
				return m_idx == r.m_idx;
			}

			bool operator!=(const Iterator& r)
			{
				return !(*this == r);
			}
		};

		class Const_Iterator : public Iterator {
			friend class Vector<T>;
			Const_Iterator(T* it) : Iterator{ it } { }

		public:
			const T& operator*()
			{
				return *Iterator::m_iter;
			}
		};

		Iterator begin()
		{
			return Iterator{ m_buffer, 0 };
		}

		Iterator begin() const
		{
			return Iterator{ m_buffer, 0 };
		}

		Iterator end()
		{
			return Iterator{ m_buffer , m_size };
		}

		Iterator end() const
		{
			return Iterator{ m_buffer , m_size };
		}

		Const_Iterator cbegin() const
		{
			return Const_Iterator{ m_buffer, 0 };
		}

		Const_Iterator cend() const
		{
			return Const_Iterator{ m_buffer, m_size };
		}

		void insert(Iterator pos, const T& val)
		{
			auto i = m_size;

			if (m_size == m_capacity) {
				auto iter_pos = pos - begin();
				inc_capacity();
				pos = begin() + iter_pos;
			}

			for (auto it = end(); it != pos; it--) {
				m_buffer[i] = m_buffer[i - 1];
				--i;
			}
			m_buffer[i] = val;
			++m_size;
		}

		void insert(Iterator pos, size_t count, const T& val)
		{
			auto i = m_size;

			if (m_size + count >= m_capacity) {
				auto iter_pos = pos - begin();
				inc_capacity(m_capacity + count + 2);
				pos = begin() + iter_pos;
			}

			for (auto it = end(); it != pos; it--) {
				m_buffer[i + count - 1] = m_buffer[i - 1];
				--i;
			}

			for (size_t i = 0; i < count; ++i) {
				*pos = val;
				m_size++;
				pos++;
			}
		}

		void insert(Iterator begin, Iterator end, T* arr);

		void erase(Iterator pos)
		{
			std::allocator_traits<Alloc>::destroy(m_alloc, &(*pos));
			for (auto iter = pos; iter != end(); ++iter) {
				*iter = std::move(*(++pos));
			}
			--m_size;
		}

		void erase(Iterator begin, Iterator end)
		{
			auto diff = end - begin;

			destroy_element(begin, diff);

			for (auto i = end; i != this->end(); ++i) {
				*(begin++) = std::move(*(i));
			}
			m_size -= diff;
		}

	};
}

#endif
