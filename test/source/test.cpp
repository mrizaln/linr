// #undef LINR_ENABLE_GETLINE    // uncomment this to use fgets instead of getline

#include <linr/buf_read.hpp>
#include <linr/read.hpp>

#include <boost/ut.hpp>

namespace ut = boost::ut;

struct Idk
{
    Idk(const Idk&)            = delete;
    Idk& operator=(const Idk&) = delete;

    Idk(Idk&&)            = default;
    Idk& operator=(Idk&&) = default;

    Idk(int i, float f)
        : m_int{ i }
        , m_float{ f }
    {
    }

    int   m_int;
    float m_float;
};

template <>
struct linr::CustomParser<Idk>
{
    Result<Idk> parse(Str str) const noexcept
    {
        auto split = linr::util::split<2>(str, ' ');
        if (not split) {
            return Error::InvalidInput;
        }

        auto i = linr::parse<int>(split->at(0)).value_or(0);
        auto f = linr::parse<float>(split->at(1)).value_or(0.0);

        return make_result<Idk>(i, f);
    }
};

// default reader, no buf
struct DefReader
{
    auto read(auto&&... args)
    {
        return linr::read(std::forward<decltype(args)>(args)...);    //
    }

    template <typename... Ts>
        requires (sizeof...(Ts) >= 1)
    auto read(auto&&... args)
    {
        return linr::read<Ts...>(std::forward<decltype(args)>(args)...);
    }

    template <typename T, std::size_t N>
        requires (N >= 1)
    auto read(auto&&... args)
    {
        return linr::read<T, N>(std::forward<decltype(args)>(args)...);
    }
};

template <typename R>
void test(R&& reader)
{
    const auto name = std::format(" [{}]", ut::reflection::type_name<R>());

    ut::test("read a string, a whole line of it" + name) = [&] {
        auto value = reader.read("insert a string: ").value();
        std::cout << "value: " << value << '\n';

        // > read call above is equivalent to
        // auto value = read<std::string>(prompt, '\n');
    };

    ut::test("read a string until delimiter reached" + name) = [&] {
        auto value = reader.template read<std::string>("a string: ").value();
        std::cout << "value: " << value << '\n';
    };

    ut::test("read a single char value" + name) = [&] {
        auto value = reader.template read<char>("a char: ").value();
        std::cout << "value: " << value << '\n';
    };

    ut::test("multiple valued read - same type" + name) = [&] {
        auto [i1, i2, i3] = reader.template read<int, int, int>("3 int: ").value();
        std::cout << "value: " << i1 << ", " << i2 << ", " << i3 << '\n';
    };

    ut::test("multiple valued read - different types" + name) = [&] {
        auto [i1, d, i2] = reader.template read<int, double, int>("an int, double and int: ").value();
        std::cout << "value: " << i1 << ", " << d << ", " << i2 << '\n';
    };

    // read until get value
    ut::test("read value until condition met" + name) = [&] {
        int result = 0;
        while (true) {
            auto value = reader.template read<int>("please enter an integer: ");
            if (value) {
                result = value.value();
                break;
            }

            using E = linr::Error;
            switch (value.error()) {
            case E::InvalidInput: std::cout << "Invalid input" << '\n'; continue;
            case E::OutOfRange: std::cout << "Input value is out of range" << '\n'; continue;
            default: break;
            }

            value = 10;
            break;
        }
        std::cout << "value: " << result << '\n';
    };

    ut::test("read custom struct" + name) = [&] {
        auto value = reader.template read<Idk>("enter int and float separated by spaces: ", '\n').value();
        std::cout << "value: " << value.m_int << " | " << value.m_float << '\n';
    };

    ut::test("read getline then parse the line into custom struct" + name) = [&] {
        auto str = reader.read("enter int and float separated by spaces: ").value();

        std::cout << ">>>>> " << str << '\n';

        auto value = linr::parse<Idk>(str).value();
        std::cout << "value: " << value.m_int << " | " << value.m_float << '\n';
    };

    ut::test("read multiple integers into array" + name) = [&] {
        auto v = reader.template read<int, 5>("5 int: ").value();
        std::cout << "value: ";
        for (auto first = true; const auto& e : v) {
            if (not std::exchange(first, false)) {
                std::cout << ", ";
            }
            std::cout << e;
        }
        std::cout << '\n';
    };
}

int main()
{
    ut::test("fundamental types can be parsed") = [] {
        static_assert(linr::Parseable<bool>);
        static_assert(linr::Parseable<char>);
        static_assert(linr::Parseable<unsigned char>);
        static_assert(linr::Parseable<signed char>);
        static_assert(linr::Parseable<int>);
        static_assert(linr::Parseable<unsigned int>);
        static_assert(linr::Parseable<long>);
        static_assert(linr::Parseable<unsigned long>);
        static_assert(linr::Parseable<long long>);
        static_assert(linr::Parseable<unsigned long long>);
    };

    ut::test("custom type can be made Parseable") = [] {
        static_assert(linr::Parseable<Idk>);    //
    };

    test(DefReader{});
    test(linr::BufReader{ 1024 });
}
