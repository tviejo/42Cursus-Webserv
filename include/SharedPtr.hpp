#pragma once
#include <algorithm>

#include <iostream>

template <typename T>
class SharedPtr
{
	private:
		T*		ptr;
		int*	ref_count;
	public:
		explicit SharedPtr(T* p  = 0) : ptr(p), ref_count(p ? new int(1) : NULL) {}
		SharedPtr(const SharedPtr& copy) : ptr(copy.ptr), ref_count(copy.ref_count) {
			++(*ref_count);
		}	
		SharedPtr<T>& operator=(const SharedPtr& copy) {
			if (*this != copy) {
				release();
				this->ptr = copy.ptr;
				this->ref_count = copy.ref_count;
				if (ref_count)
					++(*ref_count);
			}
			return *this;
		}
		T& operator*() const {
			return *ptr;
		}
		T& operator->() const {
			return ptr;
		}
		~SharedPtr() {
			release();
		}
		void	release() {
			if (ref_count) {
				if (--(*ref_count) == 0) {
					delete ptr;
					delete ref_count;
					ptr = NULL;
					ref_count = NULL;
				}
			}
		}
		int		use_count() const {
			if (ref_count)
				return *ref_count;
			else
				return 0;
		}
		bool	unique() const {
			if (ref_count)
				return *ref_count == 1;
			else
				return 0;
		}
};