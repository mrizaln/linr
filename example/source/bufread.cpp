#include "print.hpp"

#include <linr/buf_read.hpp>

template <typename... Ts>
auto read_repeat(linr::BufReader& reader, std::string_view prompt, std::string_view fail)
{
    while (true) {
        auto result = reader.read<Ts...>(prompt);
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

int main()
{
    auto reader = linr::BufReader(10);
    read_repeat<int>(reader, "input 1 int: ", "Please input an integer");
    read_repeat<int, int>(reader, "input 2 int: ", "Please input an integer");
    read_repeat<int, int, int>(reader, "input 3 int: ", "Please input an integer");
    read_repeat<int, int, int, int>(reader, "input 4 int: ", "Please input an integer");
}
