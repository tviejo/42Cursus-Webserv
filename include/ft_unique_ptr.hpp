#pragma once
#include <algorithm>
#include <iostream>

template <typename T>
class ft_unique_ptr
{
	private:
		T* ptr;
		ft_unique_ptr(const ft_unique_ptr&);	
		ft_unique_ptr& operator=(const ft_unique_ptr);
	public:
		explicit ft_unique_ptr(T* p = 0) : ptr(p) {}
		~ft_unique_ptr() {
			delete ptr;
		}
		void reset(T* p = 0) {
			if (p != ptr) {
				delete ptr;
				ptr = p;
			}
		}
		T* release() {
			T* tmp = ptr;
			ptr = 0;
			return tmp;
		}
		operator bool() const { return ptr != 0; }
		T& operator*() const { return *ptr; }
		T* operator->() const { return ptr; }
		T* get() const { return ptr; }
		void swap(ft_unique_ptr& other) {
			std::swap(ptr, other.ptr);
		}
};

template <typename T>
void	swap(ft_unique_ptr<T>& a, ft_unique_ptr<T>& b) {
	a.swap(b);
}

template <typename T>
class ft_unique_ptr<T[]>
{
	private:
		T* ptr;
		ft_unique_ptr(const ft_unique_ptr&);	
		ft_unique_ptr& operator=(const ft_unique_ptr);
	public:
		explicit ft_unique_ptr(T* p = 0) : ptr(p) {}
		~ft_unique_ptr() {
			delete[] ptr;
		}
		void reset(T* p = 0) {
			if (p != ptr) {
				delete[] ptr;
				ptr = p;
			}
		}
		T* release() {
			T* tmp = ptr;
			ptr = 0;
			return tmp;
		}
		operator bool() const { return ptr != 0; }
		T& operator[](std::size_t i) const { return ptr[i]; }
		T& operator*() const { return *ptr; }
		T* operator->() const { return ptr; }
		T* get() const { return ptr; }
		void swap(ft_unique_ptr& other) {
			std::swap(ptr, other.ptr);
		}
};

template <typename T>
void	swap(ft_unique_ptr<T[]>& a, ft_unique_ptr<T[]>& b) {
	a.swap(b); 
	}