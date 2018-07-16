#include <stdint.h>

template <typename T, typename U >
struct is_same_type;

template< typename T >
struct is_same_type<T, T>
{
  int dummy;
  is_same_type<T, T>() : dummy(0) {}
};


int main(int /*argc*/, const char* /*argv*/[])
{
    is_same_type<uint64_t, unsigned long> compiles_only_if_types_are_identical;
    return compiles_only_if_types_are_identical.dummy;
}

