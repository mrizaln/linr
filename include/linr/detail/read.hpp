#ifndef LINR_DETAIL_READ_HPP
#define LINR_DETAIL_READ_HPP

#include "linr/common.hpp"
#include "linr/detail/line_reader.hpp"
#include "linr/parser.hpp"

namespace linr::detail
{
    template <Parseable... Ts, LineReader R>
        requires (sizeof...(Ts) >= 1) and (std::movable<Ts> and ...)
    Results<Ts...> read_impl(R& reader, Opt<Str> prompt, char delim) noexcept
    {
        if (std::ferror(stdin)) {
            return make_error<Tup<Ts...>>(Error::Unknown);
        }

        if (prompt) {
            std::fwrite(prompt->data(), sizeof(Str::value_type), prompt->size(), stdout);
        }

        auto line = reader.readline();
        if (not line) {
            return make_error<Tup<Ts...>>(Error::EndOfFile);
        }

        auto parts = util::split<sizeof...(Ts)>(line->view(), delim);
        if (parts) {
            return parse_into_tuple<Ts...>(*parts);
        }
        return make_error<Tup<Ts...>>(Error::InvalidInput);
    }

    template <Parseable T, std::size_t N, LineReader R>
        requires (std::movable<T> and N > 0)
    AResults<T, N> read_impl(R& reader, Opt<Str> prompt, char delim) noexcept
    {
        if (std::ferror(stdin)) {
            return make_error<Arr<T, N>>(Error::Unknown);
        }

        if (prompt) {
            std::fwrite(prompt->data(), sizeof(Str::value_type), prompt->size(), stdout);
        }

        auto line = reader.readline();
        if (not line) {
            return make_error<Arr<T, N>>(Error::EndOfFile);
        }

        auto parts = util::split<N>(line->view(), delim);
        if (parts) {
            return parse_array<T, N>(*parts);
        }
        return make_error<Arr<T, N>>(Error::InvalidInput);
    }
}

#endif /* end of include guard: LINR_DETAIL_READ_HPP */
