// #undef LINR_ENABLE_GETLINE    // uncomment this to use fgets instead of getline

#include <linr/buf_read.hpp>
#include <linr/read.hpp>

#include <boost/ut.hpp>

#include <source_location>

namespace ut = boost::ut;

template <typename Fn>
concept Testcase = requires (Fn fn, std::string_view p, char d) {
    { fn(p, d) };
};

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

void test(auto&& read, std::source_location loc = std::source_location::current())
{
    using namespace ut::literals;

    const auto name = std::format("[{}:{}]", loc.file_name(), loc.line());

    ut::test("fundamental types can be parsed" + name) = [] {
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

    ut::test("custom type can be made Parseable" + name) = [] {
        static_assert(linr::Parseable<Idk>);    //
    };

    ut::test("read a string, a whole line of it" + name) = [&] {
        auto value = read.template operator()<>("insert a string: ").value();
        std::cout << "value: " << value << '\n';

        // > read call above is equivalent to
        // auto value = read<std::string>(prompt, '\n');
    };

    ut::test("read a string until delimiter reached" + name) = [&] {
        auto value = read.template operator()<std::string>("a string: ").value();
        std::cout << "value: " << value << '\n';
    };

    ut::test("read a single char value" + name) = [&] {
        auto value = read.template operator()<char>("a char: ").value();
        std::cout << "value: " << value << '\n';
    };

    ut::test("multiple valued read - same type" + name) = [&] {
        auto [i1, i2, i3] = read.template operator()<int, int, int>("3 int: ").value();
        std::cout << "value: " << i1 << ", " << i2 << ", " << i3 << '\n';
    };

    ut::test("multiple valued read - different types" + name) = [&] {
        auto [i1, d, i2] = read.template operator()<int, double, int>("an int, double and int: ").value();
        std::cout << "value: " << i1 << ", " << d << ", " << i2 << '\n';
    };

    // read until get value
    ut::test("read value until condition met" + name) = [&] {
        int result = 0;
        while (true) {
            auto value = read.template operator()<int>("please enter an integer: ");
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
        auto value = read.template operator()<Idk>("enter int and float separated by spaces: ", '\n').value();
        std::cout << "value: " << value.m_int << " | " << value.m_float << '\n';
    };

    ut::test("read getline then parse the line into custom struct" + name) = [&] {
        auto str = read.template operator()<>("enter int and float separated by spaces: ").value();

        std::cout << ">>>>> " << str << '\n';

        auto value = linr::parse<Idk>(str).value();
        std::cout << "value: " << value.m_int << " | " << value.m_float << '\n';
    };
}

int main()
{
    test([]<typename... T>(std::string_view prompt, char delim = ' ') {
        if constexpr (sizeof...(T) == 0) {
            return linr::read(prompt);
        } else {
            return linr::read<T...>(prompt, delim);
        }
    });

    test([reader = linr::BufReader(1024)]<typename... T>(std::string_view prompt, char delim = ' ') mutable {
        if constexpr (sizeof...(T) == 0) {
            return reader.read(prompt);
        } else {
            return reader.read<T...>(prompt, delim);
        }
    });
}
