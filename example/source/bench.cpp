#include "print.hpp"

#include <linr/buf_read.hpp>
#include <linr/read.hpp>

#include <chrono>
#include <iostream>
#include <map>
#include <ranges>
#include <utility>

enum class Method
{
    Cin,
    Nonce,
    Bufread,
};

enum class Kind
{
    Int,
    Float,
    Control,    // empty bench, measure overhead
};

struct Args
{
    Method method;
    Kind   kind;
    bool   verbose;
};

// default reader, no buf
struct DefReader
{
    template <typename... Ts>
    auto read()
    {
        return linr::read<Ts...>();
    }
};

struct CinReader
{
    template <typename... Ts>
    linr::Results<Ts...> read()
    {
        std::ios_base::sync_with_stdio(false);

        auto values  = std::tuple<Ts...>{};
        auto handler = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            ((std::cin >> std::get<Is>(values)), ...);
        };
        handler(std::index_sequence_for<Ts...>{});

        if (std::cin.fail()) {
            return linr::Error::InvalidInput;
        } else {
            return { std::move(values) };
        }
    }
};

struct EmptyReader
{
    std::size_t m_count = 0;

    template <typename... Ts>
    linr::Results<Ts...> read()
    {
        if (++m_count > 10000) {
            m_count = 0;
            return linr::Error::EndOfFile;
        }
        return { linr::Tup<Ts...>{ Ts{}... } };
    }
};

const auto kind_str = std::map<std::string, Kind>{
    { "int", Kind::Int },
    { "float", Kind::Float },
    { "control", Kind::Control },
};

std::string_view to_string(Method method)
{
    switch (method) {
    case Method::Cin: return "cin";
    case Method::Nonce: return "nonce";
    case Method::Bufread: return "bufread";
    default: [[unlikely]] return "unknown";
    }
}

std::string_view to_string(Kind kind)
{
    switch (kind) {
    case Kind::Int: return "int";
    case Kind::Float: return "float";
    case Kind::Control: return "control";
    default: [[unlikely]] return "unknown";
    }
}

template <typename T>
void bench(auto&& reader, bool print)
{
    namespace chr = std::chrono;
    using Clock   = chr::steady_clock;
    using Value   = std::tuple<T, T, T, T>;

    auto start  = Clock::now();
    auto values = std::vector<Value>{};
    auto count  = 0ul;

    values.reserve(1'000'000);
    while (true) {
        auto result = reader.template read<T, T, T, T>();
        if (not result) {
            break;
        } else {
            values.push_back(std::move(result).value());
            if (print) {
                println("value: {}", values.back());
            }
            ++count;
        }
    }

    auto elapsed = Clock::now() - start;

    using Ms = chr::milliseconds;
    auto ms  = chr::duration_cast<Ms>(elapsed);

    println("Read {} lines in {}", count, ms);
}

std::variant<Args, int> parse(int argc, char** argv)
{
    auto args = Args{
        .method  = Method::Nonce,
        .kind    = Kind::Control,
        .verbose = false,
    };

    for (auto i : std::views::iota(1, argc)) {
        auto arg = std::string{ argv[i] };

        if (arg == "--help" or arg == "-h") {
            println(
                "Usage: {} [OPTION]... KIND\n\n"
                "Options:\n"
                "   --cin       use cin instead\n"
                "   --buf       use buffered read instead\n"
                "   --verbose   Print output\n\n"
                "Kind:\n"
                "   {{ int | float | control }} (default: control)",
                argv[0]
            );
            return 0;
        }

        else if (arg == "--cin") {
            args.method = Method::Cin;
        } else if (arg == "--buf") {
            args.method = Method::Bufread;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (auto found = kind_str.find(arg); found != kind_str.end()) {
            args.kind = found->second;
        } else {
            println(stderr, "Unknown argument: {}", arg);
            return 1;
        }
    }

    return args;
}

int main(int argc, char** argv)
{
    auto maybe_args = parse(argc, argv);
    if (maybe_args.index() == 1) {
        return std::get<1>(maybe_args);
    }

    auto args = std::get<0>(maybe_args);
    println(
        "Running bench: \n"
        "- method : {}\n"
        "- kind   : {}\n"
        "- verbose: {}\n",
        to_string(args.method),
        to_string(args.kind),
        args.verbose
    );

    switch (args.kind) {
    case Kind::Int:
        switch (args.method) {
        case Method::Cin: bench<int>(CinReader{}, args.verbose); break;
        case Method::Nonce: bench<int>(DefReader{}, args.verbose); break;
        case Method::Bufread: bench<int>(linr::BufReader{ 1024 }, args.verbose); break;
        }
        break;
    case Kind::Float:
        switch (args.method) {
        case Method::Cin: bench<float>(CinReader{}, args.verbose); break;
        case Method::Nonce: bench<float>(DefReader{}, args.verbose); break;
        case Method::Bufread: bench<float>(linr::BufReader{ 1024 }, args.verbose); break;
        }
        break;
    case Kind::Control: bench<float>(EmptyReader{}, args.verbose); break;
    }
}
