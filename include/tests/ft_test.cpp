#include <iostream>
#include "SharedPtr.hpp"
#include "UniquePtr.hpp"

int main(void)
{
	ft::UniquePtr<int> un1(new int(42));
    ft::UniquePtr<int[]> un2(new int[5]);
	ft::SharedPtr<int[]> ptr(new int[5]);
	for (int i =0; i < 5; i++)
		ptr[i] = i;
	{
		ft::SharedPtr<int[]> ptr2 = ptr;
		std::cout << "ptr2: " << *ptr2 << std::endl;
		for (int i = 0; i < 5 ; i++)
			std::cout << ptr2[i] << std::endl;
	}
	std::cout << "ptr: " << *ptr << std::endl;
		for (int i = 0; i < 5 ; i++)
			std::cout << ptr[i] << std::endl;
	std::cout << "Ref count after ptr2 out of scope: " << ptr.use_count() << std::endl;
}