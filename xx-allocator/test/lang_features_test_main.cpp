#include <iostream>
#include <type_traits>


#define EXEC_TEST(test_fun_)  {                                                                    \
    std::cout << #test_fun_ << " : start" << std::endl;                                            \
    bool test_result = test_fun_();                                                                \
    if (!test_result)  exit_code = 13;                                                             \
    std::cout << #test_fun_ << " :" << (test_result?"PASS":"FAIL") << std::endl << std::endl;      \
}


////////////////////////////////////////////////////////////////////////////////
// aligned_storage_test
////////////////////////////////////////////////////////////////////////////////

template <class T>
using aligned_storage_type = std::aligned_storage_t<sizeof(T), alignof(T)>;
struct alignas(16u) alignas16_t {};

bool aligned_storage_test()
{
    aligned_storage_type<alignas16_t> aligned_storage;
    return 16u == sizeof(aligned_storage);
}

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

int main()
{
    int exit_code = 0;
    std::cout << "Raw check if compiler supports required language features." << std::endl;

    EXEC_TEST(aligned_storage_test);

    return exit_code;
}
