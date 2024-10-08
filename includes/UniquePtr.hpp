#pragma once
#include <algorithm>

template <typename T>
class UniquePtr
{
	private:
		T* ptr;
		UniquePtr(const UniquePtr&);	
		UniquePtr& operator=(const UniquePtr);
	public:
		explicit UniquePtr(T* p = 0) : ptr(p) {}
		~UniquePtr() {
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
		void swap(UniquePtr& other) {
			std::swap(ptr, other.ptr);
		}
		
};

template <typename T>
void	swap(UniquePtr<T>& a, UniquePtr<T>& b) {
	a.swap(b);
}

template <typename T>
class UniquePtr<T[]>
{
	private:
		T* ptr;
		UniquePtr(const UniquePtr&);	
		UniquePtr& operator=(const UniquePtr);
	public:
		explicit UniquePtr(T* p = 0) : ptr(p) {}
		~UniquePtr() {
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
		void swap(UniquePtr& other) {
			std::swap(ptr, other.ptr);
		}
};

template <typename T>
void	swap(UniquePtr<T[]>& a, UniquePtr<T[]>& b) {
	a.swap(b); 
}
