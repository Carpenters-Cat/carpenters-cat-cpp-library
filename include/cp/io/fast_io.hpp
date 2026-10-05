#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

namespace cp {

template <class T>
concept IoInteger = std::integral<T> && !std::same_as<T, bool> && sizeof(T) <= sizeof(unsigned long long);

template <std::size_t BufferSize = 65536>
class FastInput {
    static_assert(BufferSize > 0);
public:
    explicit FastInput(std::FILE* stream = stdin) : stream_(stream) {}
    FastInput(const FastInput&) = delete;
    FastInput& operator=(const FastInput&) = delete;

    // ASCII whitespace-delimited integers; failures leave value unchanged.
    template <IoInteger T>
    bool read(T& value) {
        int c = skip_space();
        if (c == EOF) return false;
        bool negative = false;
        if (c == '+' || c == '-') {
            negative = c == '-';
            advance();
            c = peek();
        }
        using U = std::make_unsigned_t<T>;
        const U limit = [&] {
            if constexpr (std::is_signed_v<T>) {
                return negative ? U(std::numeric_limits<T>::max()) + U(1) : U(std::numeric_limits<T>::max());
            } else {
                return std::numeric_limits<T>::max();
            }
        }();
        U magnitude = 0;
        bool valid = c >= '0' && c <= '9' && (!negative || std::is_signed_v<T>);
        while (c != EOF && !space(c)) {
            if (c < '0' || c > '9') {
                valid = false;
            } else {
                const U digit = static_cast<U>(c - '0');
                if (magnitude > limit / 10 || (magnitude == limit / 10 && digit > limit % 10)) {
                    valid = false;
                } else {
                    magnitude = static_cast<U>(magnitude * 10 + digit);
                }
            }
            advance();
            c = peek();
        }
        if (!valid) return false;
        if constexpr (std::is_signed_v<T>) {
            // Never negate the minimum signed integer or cast an out-of-range magnitude.
            value = negative && magnitude != 0 ? T(-T(magnitude - 1) - 1) : T(magnitude);
        } else {
            value = magnitude;
        }
        return true;
    }

    bool read(char& value) {
        const int c = skip_space();
        if (c == EOF) return false;
        value = static_cast<char>(c);
        advance();
        return true;
    }
    bool read(std::string& value) {
        int c = skip_space();
        if (c == EOF) return false;
        value.clear();
        while (c != EOF && !space(c)) {
            value.push_back(static_cast<char>(c));
            advance();
            c = peek();
        }
        return true;
    }
    [[nodiscard]] bool eof() { return peek() == EOF && !error(); }
    [[nodiscard]] bool error() const { return std::ferror(stream_) != 0; }

private:
    static bool space(int c) { return c == ' ' || (c >= '\t' && c <= '\r'); }
    int peek() {
        if (position_ == length_) {
            length_ = std::fread(buffer_.data(), 1, BufferSize, stream_);
            position_ = 0;
            if (length_ == 0) return EOF;
        }
        return static_cast<unsigned char>(buffer_[position_]);
    }
    void advance() { ++position_; }
    int skip_space() {
        int c = peek();
        while (c != EOF && space(c)) { advance(); c = peek(); }
        return c;
    }
    std::FILE* stream_;
    std::array<char, BufferSize> buffer_{};
    std::size_t position_ = 0, length_ = 0;
};

template <std::size_t BufferSize = 65536>
class FastOutput {
    static_assert(BufferSize > 0);
public:
    explicit FastOutput(std::FILE* stream = stdout) : stream_(stream) {}
    FastOutput(const FastOutput&) = delete;
    FastOutput& operator=(const FastOutput&) = delete;
    ~FastOutput() { flush(); }

    bool write(char value) {
        if (failed_) return false;
        if (position_ == BufferSize && !flush_buffer()) return false;
        buffer_[position_++] = value;
        return true;
    }
    bool write(std::string_view value) {
        for (char c : value) if (!write(c)) return false;
        return true;
    }
    template <IoInteger T>
    bool write(T value) {
        using U = std::make_unsigned_t<T>;
        U magnitude = static_cast<U>(value);
        if constexpr (std::is_signed_v<T>) {
            if (value < 0) {
                if (!write('-')) return false;
                magnitude = static_cast<U>(U(0) - magnitude);
            }
        }
        char digits[std::numeric_limits<U>::digits10 + 1];
        std::size_t count = 0;
        do { digits[count++] = static_cast<char>('0' + magnitude % 10); magnitude /= 10; } while (magnitude);
        while (count) if (!write(digits[--count])) return false;
        return true;
    }
    bool flush() {
        if (!flush_buffer()) return false;
        if (std::fflush(stream_) != 0) failed_ = true;
        return !failed_;
    }
    [[nodiscard]] bool error() const { return failed_ || std::ferror(stream_) != 0; }

private:
    bool flush_buffer() {
        if (failed_) return false;
        if (position_ && std::fwrite(buffer_.data(), 1, position_, stream_) != position_) failed_ = true;
        position_ = 0;
        return !failed_;
    }
    std::FILE* stream_;
    std::array<char, BufferSize> buffer_{};
    std::size_t position_ = 0;
    bool failed_ = false;
};

}  // namespace cp
