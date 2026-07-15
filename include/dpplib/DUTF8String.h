#ifndef DUTF8String_H
#define DUTF8String_H

#include <string>
#include <string_view>
#include <stdexcept>
#include <cstdint>
#include <cstddef>
#include <vector>

namespace DTools {

class DUTF8String {
    public:
        // --------------------------------- Iterator class ----------------------------------
        class iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type        = char32_t;
            using difference_type   = std::ptrdiff_t;
            using pointer           = const char32_t*;
            using reference         = char32_t;

            iterator();
            iterator(const std::string* s, size_t index);

            char32_t operator*() const;
            iterator& operator++();
            iterator operator++(int);
            iterator& operator--();
            iterator operator--(int);
            iterator& operator+=(difference_type n);
            iterator& operator-=(difference_type n);
            iterator operator+(difference_type n) const;
            iterator operator-(difference_type n) const;
            difference_type operator-(const iterator& other) const;
            bool operator==(const iterator& o) const;
            bool operator!=(const iterator& o) const;
            bool operator<(const iterator& o)  const;
            bool operator>(const iterator& o)  const;
            bool operator<=(const iterator& o) const;
            bool operator>=(const iterator& o) const;

            size_t byte_index() const;
            const std::string* container() const;

            static char32_t peek_codepoint_at(const std::string& str, size_t p, size_t &next_p);

        private:
            const std::string* str;
            size_t bytePos;
        };

        // ------------------------------- reverse_iterator class ----------------------------
        class reverse_iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type        = char32_t;
            using difference_type   = std::ptrdiff_t;

            reverse_iterator();
            explicit reverse_iterator(const iterator& i);

            char32_t operator*() const;

            reverse_iterator& operator++();
            reverse_iterator operator++(int);

            reverse_iterator& operator--();
            reverse_iterator operator--(int);

            bool operator==(const reverse_iterator& o) const;
            bool operator!=(const reverse_iterator& o) const;

        private:
            iterator it;
        };

        // ----------------------------------- Constructors ----------------------------------
        DUTF8String() = default;
        explicit DUTF8String(const std::string& utf8);
        explicit DUTF8String(std::string&& utf8) noexcept;
        explicit DUTF8String(std::string_view utf8);
        explicit DUTF8String(const char* utf8);
        explicit DUTF8String(const std::vector<char32_t>& codepoints);
        explicit DUTF8String(const std::u32string& utf32);

        DUTF8String(const DUTF8String&) = default;
        DUTF8String(DUTF8String&&) noexcept = default;
        DUTF8String& operator=(const DUTF8String&) = default;
        DUTF8String& operator=(DUTF8String&&) noexcept = default;
        ~DUTF8String() = default;

        // ------------------------------------- Operators -----------------------------------
        DUTF8String& operator=(const std::string& s);
        DUTF8String& operator=(std::string_view s);
        DUTF8String& operator=(std::string&& s) noexcept;
        DUTF8String& operator=(const char* s);
        DUTF8String& operator+=(const DUTF8String& other);
        DUTF8String& operator+=(const std::string& s);
        DUTF8String& operator+=(const char* s);
        DUTF8String& operator+=(char32_t codepoint);
        char32_t operator[](size_t index) const; // index by codepoint
        bool operator==(const std::string& s) const;
        bool operator==(std::string_view sv) const;
        bool operator==(const char* s) const;

        // --------------------------------- Conversions -------------------------------------
        std::string to_string() const;
        std::string_view to_string_view() const noexcept;
        const char* c_str() const noexcept;
        std::u32string to_utf32() const;
        static std::string utf32_to_utf8(const std::u32string& src);
        static std::u32string utf8_to_utf32(const std::string& src);

        // -------------------------------- Modifications ------------------------------------
        DUTF8String& assign(const DUTF8String& other);
        DUTF8String& assign(DUTF8String&& other) noexcept;
        DUTF8String& assign(const std::string& s);
        DUTF8String& assign(std::string&& s) noexcept;
        DUTF8String& assign(const char* s);
        DUTF8String& assign(const char* s, size_t count);
        size_t append_codepoint(char32_t codepoint);
        void insert_codepoint(size_t index, char32_t codepoint);
        void insert(size_t index, const std::string& str);
        void replace_codepoint(char32_t target, char32_t replacement);
        void replace(const std::string& search, const std::string& replace_with_naive);
        void erase_codepoint(size_t index);
        DUTF8String& erase(size_t index_cp, size_t count_cp = SIZE_MAX);
        DUTF8String& erase(iterator it_start, iterator it_end);
        void pop_back();

        // ------------------------------- Interrogations ------------------------------------
        std::string substr(size_t start_cp, size_t count_cp = SIZE_MAX) const; // TODO: deprecate?
        std::string_view substr_view(size_t start_cp, size_t count_cp = SIZE_MAX) const;
        size_t find(char32_t target, size_t start_cp = 0) const;
        size_t find(const std::string& sub, size_t start_cp = 0) const;
        bool empty() const noexcept;
        void clear();
        size_t length() const; // codepoint count (cached)
        size_t codepoint_index_to_byte_index(size_t cpIndex) const; // map codepoint index -> byte index
        void generate_cp_index_list(std::vector<size_t>& destList);

        // --------------------- KMP-based search/replace on codepoints ----------------------
        size_t find_kmp(const std::string& sub, size_t start_cp = 0) const;
        void replace_kmp(const std::string& search, const std::string& replace_with);

        // ------------------------------------- Iterators -----------------------------------
        iterator begin() const;
        iterator end() const;
        reverse_iterator rbegin() const;
        reverse_iterator rend() const;

        // ---------------------------------- Public helpers ---------------------------------
        static bool is_unicode(int codepoint);

    private:
        std::string data;
        mutable size_t cached_len = 0;
        mutable bool cache_valid = false;

        // ---------------------------------- Private helpers --------------------------------
        void invalidate_cache() const;
        static size_t utf8_char_length(unsigned char c) noexcept;
        static char32_t decode_utf8_at(const std::string& s, size_t p, size_t &next_p);
        static void encode_utf8(char32_t codepoint, std::string& out);
        static void move_prev_utf8(const std::string& s, size_t& index);
        static std::vector<int> build_lps(const std::u32string& pat);

}; // end class DUTF8String

// --------------- non-member concatenation helpers ----------------------
DUTF8String operator+(const DUTF8String& lhs, const DUTF8String& rhs);
DUTF8String operator+(const std::string& lhs, const DUTF8String& rhs);
DUTF8String operator+(const char* lhs, const DUTF8String& rhs);
} // end namespace DTools

#endif
