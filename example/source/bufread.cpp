#include "print.hpp"

#include <linr/buf_read.hpp>

template <typename Fn, typename... Ts>
auto repeat_impl(std::string_view fail, Fn read)
{
    while (true) {
        auto result = read();
        if (result) {
            println("{}", result.value());
            return;
        }

        if (result.error() == linr::Error::EndOfFile) {
            println("stdin EOF reached!");
            return;
        }
        println("{} [{}]", fail, to_string(result.error()));
    }
}

template <typename... Ts>
auto read_repeat(linr::BufReader& reader, std::string_view prompt, std::string_view fail)
{
    repeat_impl(fail, [&] { return reader.read<Ts...>(prompt); });
}

template <typename T, std::size_t N>
auto read_repeat(linr::BufReader& reader, std::string_view prompt, std::string_view fail)
{
    repeat_impl(fail, [&] { return reader.read<T, N>(prompt); });
}

int main()
{
    auto reader = linr::BufReader(10);

    // tuple
    read_repeat<int>(reader, "input 1 int: ", "Please input an integer");
    read_repeat<int, int>(reader, "input 2 int: ", "Please input an integer");
    read_repeat<int, int, int>(reader, "input 3 int: ", "Please input an integer");
    read_repeat<int, int, int, int>(reader, "input 4 int: ", "Please input an integer");

    // array
    read_repeat<int, 1>(reader, "input 1 int: ", "Please input an integer");
    read_repeat<int, 2>(reader, "input 2 int: ", "Please input an integer");
    read_repeat<int, 3>(reader, "input 3 int: ", "Please input an integer");
    read_repeat<int, 4>(reader, "input 4 int: ", "Please input an integer");
}
