#include "SharedPtr.hpp"
#include <iostream>

int main() {
    SharedPtr<int> sp1(new int(10));  // Creates a new shared_ptr with a new int
    std::cout << "sp1 use_count: " << sp1.use_count() << std::endl;  // Output: 1

    {
        SharedPtr<int> sp2 = sp1;  // Copy constructor, both share ownership
        std::cout << "sp2 use_count: " << sp2.use_count() << std::endl;  // Output: 2
        std::cout << "sp1 use_count: " << sp1.use_count() << std::endl;  // Output: 2
    }  // sp2 goes out of scope and the ref_count decreases
	SharedPtr<int> sp3 = sp1;
	std::cout << "sp1 use_count: " << sp1.use_count() << std::endl;
	std::cout << "sp3 use_count: " << sp3.use_count() << std::endl;
	std::cout << "sp3 ptr: " << *sp3 << std::endl;
    std::cout << "sp1 use_count after sp3 released: " << sp1.use_count() << std::endl;  // Output: 1
    return 0;
}