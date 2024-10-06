#include <iostream>
#include <cassert>
#include "ft_unique_ptr.hpp" // Assume the ft_unique_ptr implementation is in this file

// A simple class for testing
class TestClass {
public:
    TestClass() : value(0) { std::cout << "TestClass constructed\n"; }
    explicit TestClass(int v) : value(v) { std::cout << "TestClass constructed with value " << v << "\n"; }
    ~TestClass() { std::cout << "TestClass destructed\n"; }
    int getValue() const { return value; }
    void setValue(int v) { value = v; }
private:
    int value;
};

void test_single_object() {
    std::cout << "Testing single object ft_unique_ptr:\n";
    
    // Test constructor and destructor
    {
        ft_unique_ptr<TestClass> ptr(new TestClass(10));
        assert(ptr->getValue() == 10);
    }
    std::cout << "Constructor and destructor test passed\n";

    // Test reset
    {
        ft_unique_ptr<TestClass> ptr(new TestClass(10));
        ptr.reset(new TestClass(20));
        assert(ptr->getValue() == 20);
        ptr.reset();
        assert(!ptr);
    }
    std::cout << "Reset test passed\n";

    // Test release
    {
        ft_unique_ptr<TestClass> ptr(new TestClass(30));
        TestClass* raw_ptr = ptr.release();
        assert(!ptr);
        assert(raw_ptr->getValue() == 30);
        delete raw_ptr;
    }
    std::cout << "Release test passed\n";

    // Test boolean conversion
    {
        ft_unique_ptr<TestClass> ptr1(new TestClass());
        ft_unique_ptr<TestClass> ptr2;
        assert(ptr1);
        assert(!ptr2);
    }
    std::cout << "Boolean conversion test passed\n";

    // Test get
    {
        TestClass* raw_ptr = new TestClass(40);
        ft_unique_ptr<TestClass> ptr(raw_ptr);
        assert(ptr.get() == raw_ptr);
    }
    std::cout << "Get test passed\n";

    // Test swap
    {
        ft_unique_ptr<TestClass> ptr1(new TestClass(50));
        ft_unique_ptr<TestClass> ptr2(new TestClass(60));
        swap(ptr1, ptr2);
        assert(ptr1->getValue() == 60);
        assert(ptr2->getValue() == 50);
    }
    std::cout << "Swap test passed\n";
}

void test_array() {
    std::cout << "\nTesting array ft_unique_ptr:\n";
    
    // Test constructor and destructor
    {
        ft_unique_ptr<int[]> ptr(new int[5]);
        for (int i = 0; i < 5; ++i) {
            ptr[i] = i * 10;
        }
        assert(ptr[3] == 30);
    }
    std::cout << "Constructor and destructor test passed\n";

    // Test reset
    {
        ft_unique_ptr<int[]> ptr(new int[3]);
        ptr.reset(new int[4]);
        ptr[3] = 40;
        assert(ptr[3] == 40);
        ptr.reset();
        assert(!ptr);
    }
    std::cout << "Reset test passed\n";

    // Test release
    {
        ft_unique_ptr<int[]> ptr(new int[3]);
        int* raw_ptr = ptr.release();
        assert(!ptr);
        delete[] raw_ptr;
    }
    std::cout << "Release test passed\n";

    // Test boolean conversion and get
    {
        ft_unique_ptr<int[]> ptr1(new int[3]);
        ft_unique_ptr<int[]> ptr2;
        assert(ptr1);
        assert(!ptr2);
        assert(ptr1.get() != nullptr);
        assert(ptr2.get() == nullptr);
    }
    std::cout << "Boolean conversion and get test passed\n";

    // Test swap
    {
        ft_unique_ptr<int[]> ptr1(new int[1]);
        ft_unique_ptr<int[]> ptr2(new int[1]);
        ptr1[0] = 100;
        ptr2[0] = 200;
        swap(ptr1, ptr2);
        assert(ptr1[0] == 200);
        assert(ptr2[0] == 100);
    }
    std::cout << "Swap test passed\n";
}

int main() {
    test_single_object();
    test_array();
    std::cout << "\nAll tests passed successfully!\n";
    return 0;
}