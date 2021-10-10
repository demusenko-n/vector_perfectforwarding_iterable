#pragma once
#include "my_vector_exception.h"
#include <memory>

namespace my_vector
{
	template <class T, class Alloc_T = std::allocator<T>>
	class my_vector
	{
		Alloc_T allocator_;
		T* arr_;
		size_t size_;
		size_t capacity_;

	public:
		class iterator
		{
		public:
			using iterator_category = std::contiguous_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = value_type*;
			using reference = value_type&;

			iterator(pointer ptr) : m_ptr(ptr) {}

			reference operator*() const { return *m_ptr; }
			pointer operator->() { return m_ptr; }

			// Prefix increment
			iterator& operator++() { ++m_ptr; return *this; }

			// Postfix increment
			iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

			friend bool operator== (const iterator& a, const iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const iterator& a, const iterator& b) { return a.m_ptr != b.m_ptr; };

		private:
			pointer m_ptr;
		};
		class constant_iterator
		{
		public:
			using iterator_category = std::contiguous_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using const_reference = const T&;
			using const_pointer = const T*;

			explicit constant_iterator(pointer ptr) : m_ptr(ptr) {}

			const_reference operator*() const { return *m_ptr; }
			const_pointer operator->() { return m_ptr; }

			// Prefix increment
			constant_iterator& operator++() { ++m_ptr; return *this; }

			// Postfix increment
			constant_iterator operator++(int) { constant_iterator tmp = *this; ++(*this); return tmp; }

			friend bool operator== (const constant_iterator& a, const constant_iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const constant_iterator& a, const constant_iterator& b) { return a.m_ptr != b.m_ptr; };

		private:
			pointer m_ptr;
		};

		explicit my_vector(size_t size, const T& default_val = T())
		{
			capacity_ = size;
			arr_ = allocator_.allocate(capacity_);
			size_ = size;
			construct_with_value(0, size_, default_val);
		}
		my_vector()noexcept
		{
			arr_ = nullptr;
			size_ = capacity_ = 0;
		}
		my_vector(const my_vector& other)
		{
			capacity_ = other.capacity_;
			arr_ = allocator_.allocate(capacity_);
			size_ = other.size_;
			for (int i = 0; i < size_; ++i)
			{
				std::construct_at(&arr_[i], other.arr_[i]);
			}
		}
		my_vector(my_vector&& other) noexcept : allocator_(std::move(other.allocator_))
		{
			capacity_ = other.capacity_;
			size_ = other.size_;
			arr_ = other.arr_;

			other.arr_ = nullptr;
			other.capacity_ = other.size_ = 0;
		}
		~my_vector()
		{
			free();
		}
		my_vector& operator=(my_vector&& other) noexcept
		{
			free();
			allocator_ = std::move(other.allocator_);
			arr_ = other.arr_;
			capacity_ = other.capacity_;
			size_ = other.size_;
			other.arr_ = nullptr;
			other.capacity_ = 0;
			other.size_ = 0;
			return *this;
		}
		my_vector& operator=(const my_vector& other)
		{
			if (this == &other) return *this;

			free();
			capacity_ = other.capacity_;
			arr_ = allocator_.allocate(capacity_);
			size_ = other.size_;
			for (size_t i = 0; i < size_; ++i)
			{
				std::construct_at(&arr_[i], other.arr_[i]);
			}
			return *this;
		}
		void clear()noexcept
		{
			std::destroy_n(arr_, size_);
			size_ = 0;
		}
		void reserve(size_t new_capacity)
		{
			if (capacity_ >= new_capacity)
			{
				return;
			}

			T* new_arr = allocator_.allocate(new_capacity);

			for (size_t i = 0; i < size_; ++i)
			{
				std::construct_at(&new_arr[i], std::move(arr_[i]));
			}
			std::destroy_n(arr_, size_);
			allocator_.deallocate(arr_, capacity_);
			arr_ = new_arr;
			capacity_ = new_capacity;
		}
		void resize(size_t new_size)
		{
			reserve(new_size);
			if (new_size > size_)
			{
				construct_with_value(size_, new_size, T());
			}
			else if (new_size < size_)
			{
				std::destroy(&arr_[new_size], &arr_[size_]);
			}
			size_ = new_size;
		}
		void resize(size_t new_size, const T& default_val)
		{
			reserve(new_size);
			if (new_size > size_)
			{
				construct_with_value(size_, new_size, default_val);
			}
			else if (new_size < size_)
			{
				std::destroy(&arr_[new_size], &arr_[size_]);
			}
			size_ = new_size;
		}
		void shrink_to_fit()
		{
			if (size_ == capacity_)
			{
				return;
			}

			T* new_arr = allocator_.allocate(size_);
			for (size_t i = 0; i < size_; ++i)
			{
				std::construct_at(&new_arr[i], std::move(arr_[i]));
			}
			std::destroy_n(arr_, size_);
			allocator_.deallocate(arr_, capacity_);

			arr_ = new_arr;
			capacity_ = size_;
		}

		void push_back(T&& value)
		{
			emplace_back(std::move(value));
		}
		void push_back(const T& value)
		{
			emplace_back(value);
		}
		template <typename... Ts>
		T& emplace_back(Ts&&... args)
		{
			if (size_ == capacity_)
			{
				reserve(calculate_capacity(size_ + 1));
			}
			
			T* temp = &arr_[size_];
			std::construct_at(&arr_[size_++], std::forward<Ts>(args)...);
			return *temp;
		}
		void pop_back()
		{
			if (size_ == 0)
			{
				throw my_vector_exception("Vector is empty");
			}
			std::destroy_at(&arr_[--size_]);
		}

		[[nodiscard]] Alloc_T get_allocator()const noexcept
		{
			return allocator_;
		}

		[[nodiscard]] const T& at(size_t index)const
		{
			if (index >= size_)
			{
				throw my_vector_exception("index out of range");
			}
			return arr_[index];
		}
		T& at(size_t index)
		{
			if (index >= size_)
			{
				throw my_vector_exception("index out of range");
			}
			return arr_[index];
		}
		const T& operator[](size_t index)const noexcept
		{
			return arr_[index];
		}
		T& operator[](size_t index)noexcept
		{
			return arr_[index];
		}
		[[nodiscard]] const T* data()const noexcept
		{
			return arr_;
		}
		T* data()noexcept
		{
			return arr_;
		}
		[[nodiscard]] bool empty()const noexcept
		{
			return size_ == 0;
		}
		[[nodiscard]] size_t max_size()const noexcept
		{
			return std::allocator_traits<Alloc_T>::max_size(allocator_);
		}
		[[nodiscard]] size_t size()const noexcept
		{
			return size_;
		}
		[[nodiscard]] size_t capacity()const noexcept
		{
			return capacity_;
		}
		T& front()
		{
			if (size_ == 0)
			{
				throw my_vector_exception("Vector is empty!");
			}
			return arr_[0];
		}
		[[nodiscard]] const T& front()const
		{
			if (size_ == 0)
			{
				throw my_vector_exception("Vector is empty!");
			}
			return arr_[0];
		}
		T& back()
		{
			if (size_ == 0)
			{
				throw my_vector_exception("Vector is empty!");
			}
			return arr_[size_ - 1];
		}
		[[nodiscard]] const T& back()const
		{
			if (size_ == 0)
			{
				throw my_vector_exception("Vector is empty!");
			}
			return arr_[size_ - 1];
		}
		void swap(my_vector& other) noexcept
		{
			std::swap(other.arr_, arr_);
			std::swap(other.capacity_, capacity_);
			std::swap(other.size_, size_);
			std::swap(other.allocator_, allocator_);
		}
		iterator begin()
		{
			return iterator(arr_ == nullptr ? nullptr : &arr_[0]);
		}
		iterator end()
		{
			return iterator(arr_ == nullptr ? nullptr : &arr_[size_]);
		}
		constant_iterator cbegin()
		{
			return constant_iterator(arr_ == nullptr ? nullptr : &arr_[size_]);
		}
		constant_iterator cend()
		{
			return constant_iterator(arr_ == nullptr ? nullptr : &arr_[size_]);
		}

	private:
		[[nodiscard]] size_t calculate_capacity(const size_t new_size) const
		{
			const size_t old_capacity = capacity_;
			const size_t max = max_size();


			if (old_capacity > max - old_capacity / 2)
			{
				return max; // geometric growth would overflow
			}
			const size_t new_geometric_capacity = old_capacity + old_capacity / 2;

			if (new_geometric_capacity < new_size) {
				return new_size; // geometric growth would be insufficient
			}

			return new_geometric_capacity;
		}


		void construct_with_value(int start, int end, const T& val)
		{
			for (int i = start; i < end; ++i)
			{
				std::construct_at(&arr_[i], val);
			}
		}
		void free()noexcept
		{
			if (arr_ != nullptr)
			{
				std::destroy_n(arr_, size_);
				allocator_.deallocate(arr_, capacity_);
			}
			size_ = capacity_ = 0;
		}
	};
}
