#include <iostream>
#include <tuple>
#include <map>
#include <functional>
#include "../factory.h"

struct foo : dp14::code<float>
{
    DEFINE_KEY(foo)
    foo(float value)
    {
        std::cout << "code 1 with value: " << value << std::endl;
    }
};

struct reb : dp14::code<float>
{
    DEFINE_KEY(reb)
    reb(float value)
    {
        std::cout << "code 2 with value: " << value << std::endl;
    }
};

namespace regs
{
    dp14::repository<float>::registrator<foo> reg1;
    dp14::repository<float>::registrator<reb> reg2;
}

int main()
{
    auto t = std::make_tuple(1.0, 2.0, 3.0, 4.0, 5.0);
    mc::foreach_tuple(t, [&](auto elem)
    {
        dp14::repository<float>::instance().create("foo", elem);
        dp14::repository<float>::instance().create("reb", elem);
    });

    return 0;
}
