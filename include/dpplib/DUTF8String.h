#ifndef DUTF8STRING_H
#define DUTF8STRING_H

#include <string>
#include <stdexcept>
#include <cstdint>
#include <cstddef>
#include <vector>

namespace DTools {

class DUTF8String {
public:
    // ---------------------------------------------------------------------
    // Iterator (ibrido: confronti byte-based, avanzamento/count codepoint-based)
    // ---------------------------------------------------------------------
    class iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = char32_t;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const char32_t*;
        using reference         = char32_t;

        iterator() : str(nullptr), bytePos(0) {}
        iterator(const std::string* s, size_t pos) : str(s), bytePos(pos) {}

        // dereference -> returns current codepoint (reads from bytePos)
        char32_t operator*() const;

        // pre-increment (advance one codepoint)
        iterator& operator++();

        // post-increment
        iterator operator++(int);

        // pre-decrement (move to previous codepoint)
        iterator& operator--();

        // post-decrement
        iterator operator--(int);

        // advance/rewind by n codepoints (loops)
        iterator& operator+=(difference_type n);
        iterator& operator-=(difference_type n);
        iterator operator+(difference_type n) const;
        iterator operator-(difference_type n) const;

        // distance in codepoints between two iterators (this - other)
        difference_type operator-(const iterator& other) const;

        // comparisons (fast, byte-based)
        bool operator==(const iterator& o) const;
        bool operator!=(const iterator& o) const;
        bool operator<(const iterator& o)  const;
        bool operator>(const iterator& o)  const;
        bool operator<=(const iterator& o) const;
        bool operator>=(const iterator& o) const;

        // expose underlying byte index and container
        size_t byte_index() const { return bytePos; }
        const std::string* container() const { return str; }

        // helper to peek a codepoint at byte index p, returns codepoint and sets next_p
        static char32_t peek_codepoint_at(const std::string& str, size_t p, size_t &next_p);

    private:
        const std::string* str;
        size_t bytePos;
    };

    // ---------------------------------------------------------------------
    // reverse_iterator built on top of iterator
    // ---------------------------------------------------------------------
    class reverse_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = char32_t;
        using difference_type   = std::ptrdiff_t;

        reverse_iterator() : it() {}
        explicit reverse_iterator(const iterator& i) : it(i) {}

        char32_t operator*() const { iterator tmp = it; --tmp; return *tmp; }

        reverse_iterator& operator++() { --it; return *this; }
        reverse_iterator operator++(int) { reverse_iterator tmp = *this; ++(*this); return tmp; }

        reverse_iterator& operator--() { ++it; return *this; }
        reverse_iterator operator--(int) { reverse_iterator tmp = *this; --(*this); return tmp; }

        bool operator==(const reverse_iterator& o) const { return it == o.it; }
        bool operator!=(const reverse_iterator& o) const { return it != o.it; }

    private:
        iterator it;
    };

    // ---------------------------------------------------------------------
    // Constructors / assignment
    // ---------------------------------------------------------------------
    DUTF8String() = default;
    explicit DUTF8String(const std::string& utf8);
    explicit DUTF8String(std::string&& utf8) noexcept;
    explicit DUTF8String(const char* utf8);
    explicit DUTF8String(const std::vector<char32_t>& cps);
    explicit DUTF8String(const std::u32string& utf32);

    DUTF8String(const DUTF8String&) = default;
    DUTF8String(DUTF8String&&) noexcept = default;
    DUTF8String& operator=(const DUTF8String&) = default;
    DUTF8String& operator=(DUTF8String&&) noexcept = default;
    ~DUTF8String() = default;

    // ---------------------------------------------------------------------
    // Basic access / conversions
    // ---------------------------------------------------------------------
    std::string to_string() const;
    std::string_view to_string_view() const noexcept;
    const char* c_str() const noexcept;

    bool empty() const noexcept;
    void clear();

    // codepoint count (cached)
    size_t length() const;

    // operator[] index by codepoint
    char32_t operator[](size_t index) const;

    // append / insert / erase codepoints
    void append_codepoint(char32_t cp);
    void insert_codepoint(size_t index, char32_t cp); // insert at codepoint index
    void erase_codepoint(size_t index);                // erase single codepoint

    // erase range by codepoint indices
    DUTF8String& erase(size_t pos_cp, size_t count_cp = SIZE_MAX);
    DUTF8String& erase(iterator it_start, iterator it_end);

    // substrings (start/count in codepoints)
    std::string substr(size_t start_cp, size_t count_cp = SIZE_MAX) const;

    // find (single codepoint or substring), returns codepoint index or SIZE_MAX
    size_t find(char32_t target, size_t start_cp = 0) const;
    size_t find(const std::string& sub, size_t start_cp = 0) const;

    // replace (char or substring)
    void replace(char32_t target, char32_t replacement);
    void replace(const std::string& search, const std::string& replace_with);

    // UTF-32 conversions
    std::u32string to_utf32() const;

    // iterators
    iterator begin() const;
    iterator end() const;
    reverse_iterator rbegin() const;
    reverse_iterator rend() const;

    // non-member concatenation helpers will be declared outside namespace

private:
    std::string data;

    // cache for length (in codepoints)
    mutable size_t cached_len = 0;
    mutable bool cache_valid = false;
    void invalidate_cache() const { cache_valid = false; }

    // helpers
    static size_t utf8_char_length(unsigned char c) noexcept;
    static char32_t decode_utf8_at(const std::string& s, size_t p, size_t &next_p);
    static void encode_utf8(char32_t cp, std::string& out);
    static void move_prev_utf8(const std::string& s, size_t& pos);

}; // class DUTF8String

// non-member concatenation helpers (declarations)
DUTF8String operator+(const DUTF8String& lhs, const DUTF8String& rhs);
DUTF8String operator+(const std::string& lhs, const DUTF8String& rhs);
DUTF8String operator+(const char* lhs, const DUTF8String& rhs);

} // namespace DTools

#endif // DUTF8STRING_H
