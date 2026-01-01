#ifndef LINR_BUF_READER_HPP
#define LINR_BUF_READER_HPP

#include "linr/common.hpp"
#include "linr/detail/read.hpp"
#include "linr/parser.hpp"

#include <algorithm>

namespace linr
{
    class BufReader
    {
    public:
        BufReader(std::size_t size) noexcept
            : m_stream{ stdin }
            , m_reader{ size }
        {
        }

        BufReader(std::FILE* stream, std::size_t size) noexcept
            : m_stream{ stream }
            , m_reader{ size }
        {
        }

        /**
         * @brief Read multiple values from stream as tuple.
         *
         * @param prompt The prompt.
         * @param delim Delimiter, only `char` so you can't use unicode.
         */
        template <Parseable... Ts>
            requires (sizeof...(Ts) > 1) and (std::movable<Ts> and ...)
        Results<Ts...> read(Opt<Str> prompt = std::nullopt, char delim = ' ') noexcept
        {
            return detail::read_impl<Ts...>(m_stream, m_reader, prompt, delim);
        }

        /**
         * @brief Read a single value from stream.
         *
         * @param prompt The prompt.
         * @param delim Delimiter, only `char` so you can't use unicode.
         */
        template <Parseable T>
            requires std::movable<T>
        Result<T> read(Opt<Str> prompt = std::nullopt, char delim = ' ') noexcept
        {
            auto result = detail::read_impl<T>(m_stream, m_reader, prompt, delim);
            if (result) {
                return make_result<T>(std::get<0>(std::move(result).value()));
            }
            return make_error<T>(result.error());
        }

        /**
         * @brief Read a string until '\n' is found (aka getline)
         *
         * @param prompt The prompt.
         */
        Result<std::string> read(Opt<Str> prompt = std::nullopt) noexcept
        {
            auto result = detail::read_impl<std::string>(m_stream, m_reader, prompt, '\n');
            if (result) {
                return make_result<std::string>(std::get<0>(std::move(result).value()));
            }
            return make_error<std::string>(result.error());
        }

        /**
         * @brief Read multiple values from stream as array.
         *
         * @param prompt The prompt.
         * @param delim Delimiter, only `char` so you can't use unicode.
         */
        template <typename T, std::size_t N>
        AResults<T, N> read(Opt<Str> prompt = std::nullopt, char delim = ' ') noexcept
        {
            return detail::read_impl<T, N>(m_stream, m_reader, prompt, delim);
        }

        void set_stream(std::FILE* stream) { m_stream = stream; }

        std::FILE* get_stream() const { return m_stream; }

    private:
        std::FILE*        m_stream;
        detail::BufReader m_reader;
    };
}

#endif /* end of include guard: LINR_BUF_READER_HPP */
