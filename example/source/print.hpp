#ifndef PRINT_WERS7D5VRFAWEI
#define PRINT_WERS7D5VRFAWEI

#include <cstdio>
#include <format>
#include <utility>

template <typename... Args>
void print(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args)
{
    auto string = std::format(fmt, std::forward<Args>(args)...);
    std::fprintf(stream, "%s", string.c_str());
}

template <typename... Args>
void println(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args)
{
    print(stream, fmt, std::forward<Args>(args)...);
    std::putc('\n', stream);
}

template <typename... Args>
void print(std::format_string<Args...> fmt, Args&&... args)
{
    print(stdout, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void println(std::format_string<Args...> fmt, Args&&... args)
{
    println(stdout, fmt, std::forward<Args>(args)...);
}

#if __cplusplus < 202302L
// tuple formatter, not available before C++23
template <typename... Ts>
struct std::formatter<std::tuple<Ts...>> : std::formatter<std::string_view>
{
    auto format(const std::tuple<Ts...>& tup, auto& ctx) const
    {
        ctx.out() = '[';

        auto format = [&]<std::size_t... I>(std::index_sequence<I...>) {
            ((I == 0 ? void() : void(ctx.out() = ", "), std::formatter<Ts>{}.format(std::get<I>(tup), ctx)),
             ...);
        };
        format(std::index_sequence_for<Ts...>{});

        ctx.out() = ']';
        return ctx.out();
    }
};

// ranges formatter, not available before C++23
template <std::ranges::range R>
struct std::formatter<R> : std::formatter<std::string_view>
{
    auto format(const R& range, auto& ctx) const
    {
        ctx.out() = '[';

        auto begin = std::ranges::begin(range);
        auto end   = std::ranges::end(range);
        auto first = true;

        while (begin != end) {
            if (not std::exchange(first, false)) {
                ctx.out() = ", ";
            }
            std::formatter<std::ranges::range_value_t<R>>{}.format(*begin, ctx);
            ++begin;
        }

        ctx.out() = ']';
        return ctx.out();
    }
};
#endif

#endif /* PRINT_WERS7D5VRFAWEI */
