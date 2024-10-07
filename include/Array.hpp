#pragma once
#include <algorithm>

template <typename T, uint32_t N>
class Array
{
	private:
		const uint32_t	_max_size;
		uint32_t		_size;
		T				_array[N];
	public:
		Array() : _size(0), _max_size(N) {};
		~Array();
		Array(const Array& copy) : _size(copy._size), _max_size(copy._max_size) {
			*this = copy;
		};
		Array& operator=(const Array& copy) {
			if (*this != copy){
				for (uint32_t i = 0; i < _max_size; i++) {
					_array[i] = copy._array[i];
				}
			}
			return *this;
		};
		T& operator[](uint32_t index) {
			if (index >= _max_size)
				throw std::out_of_range("Index value out of array bounds");
			return _array[index];
		};
		uint32_t		size() const {
			return _size;
		};
		uint32_t		max_size() const {
			return _max_size;
		};
		bool			empty() const {
			return _size == 0;
		};
		T&				front() const {
			return _array[0];
		};
		T&				back() const {
			return _array[_size - 1];
		};
		T&				at(uint32_t index) {
			if (index >= _max_size)
				throw std::out_of_range("Index value out of array bounds");
			return _array[index];
		};
		void			fill(const T& value)	{
			for (uint32_t i = 0; i < _max_size; i++)
			{
				_array[i] = value;
				_size++;
			}
		};
		void			swap(Array &other) {
			if (_max_size != other._max_size)
				throw std::out_of_range("Arrays of different sizes");
			for (uint32_t i = 0; i < _max_size; i++) {
				T temp = _array[i];
				_array[i] = other._array[i];
				other._array[i] = temp;
			}
			uint32_t	temp_size = _size;
			_size = other._size;
			other._size = temp_size;
		};
		T*		begin() {
			return _array;
		};
		T*		end() {
			return _array + _size;
		};
};


