#include "dpplib/DUTF8String.h"
#include <algorithm>

namespace DTools {

// ----------------------------- Constructors -----------------------------

DUTF8String::DUTF8String(const std::string& utf8) : data(utf8) {
    invalidate_cache();
}

DUTF8String::DUTF8String(std::string&& utf8) noexcept : data(std::move(utf8)) {
    invalidate_cache();
}

DUTF8String::DUTF8String(const char* utf8) : data(utf8 ? utf8 : "") {
    invalidate_cache();
}

DUTF8String::DUTF8String(const std::vector<char32_t>& cps) {
    for (char32_t cp : cps) encode_utf8(cp, data);
    invalidate_cache();
}

DUTF8String::DUTF8String(const std::u32string& utf32) {
    for (char32_t cp : utf32) encode_utf8(cp, data);
    invalidate_cache();
}

// ----------------------------- Basic helpers -----------------------------

size_t DUTF8String::utf8_char_length(unsigned char c) noexcept {
    if ((c & 0x80) == 0x00) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

char32_t DUTF8String::decode_utf8_at(const std::string& s, size_t p, size_t &next_p) {
    next_p = p;
    if (p >= s.size()) return 0;
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(s.data() + p);
    unsigned char c0 = ptr[0];
    if ((c0 & 0x80) == 0x00) {
        next_p = p + 1;
        return c0;
    }
    if ((c0 & 0xE0) == 0xC0) {
        if (p + 1 >= s.size()) throw std::runtime_error("Truncated UTF-8");
        next_p = p + 2;
        return ((ptr[0] & 0x1F) << 6) | (ptr[1] & 0x3F);
    }
    if ((c0 & 0xF0) == 0xE0) {
        if (p + 2 >= s.size()) throw std::runtime_error("Truncated UTF-8");
        next_p = p + 3;
        return ((ptr[0] & 0x0F) << 12) | ((ptr[1] & 0x3F) << 6) | (ptr[2] & 0x3F);
    }
    if ((c0 & 0xF8) == 0xF0) {
        if (p + 3 >= s.size()) throw std::runtime_error("Truncated UTF-8");
        next_p = p + 4;
        return ((ptr[0] & 0x07) << 18) | ((ptr[1] & 0x3F) << 12) |
               ((ptr[2] & 0x3F) << 6) | (ptr[3] & 0x3F);
    }
    throw std::runtime_error("Invalid UTF-8 sequence");
}

void DUTF8String::encode_utf8(char32_t cp, std::string& out) {
    if (cp <= 0x7F) {
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
}

void DUTF8String::move_prev_utf8(const std::string& s, size_t& pos) {
    if (pos == 0) return;
    size_t i = pos - 1;
    // move back to the start byte (not continuation 10xxxxxx)
    while (i > 0 && (static_cast<unsigned char>(s[i]) & 0xC0) == 0x80) --i;
    pos = i;
}

// ----------------------------- iterator helpers -----------------------------

char32_t DUTF8String::iterator::peek_codepoint_at(const std::string& str, size_t p, size_t &next_p) {
    return DUTF8String::decode_utf8_at(str, p, next_p);
}

// ----------------------------- iterator implementation (hybrid) -----------------------------

char32_t DUTF8String::iterator::operator*() const {
    if (!str) return 0;
    size_t np = bytePos;
    return peek_codepoint_at(*str, bytePos, np);
}

DUTF8String::iterator& DUTF8String::iterator::operator++() {
    if (!str) return *this;
    if (bytePos >= str->size()) return *this;
    unsigned char c = static_cast<unsigned char>((*str)[bytePos]);
    bytePos += DUTF8String::utf8_char_length(c);
    return *this;
}

DUTF8String::iterator DUTF8String::iterator::operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
}

DUTF8String::iterator& DUTF8String::iterator::operator--() {
    if (!str) return *this;
    if (bytePos == 0) return *this;
    size_t p = bytePos - 1;
    while (p > 0 && (static_cast<unsigned char>((*str)[p]) & 0xC0) == 0x80) --p;
    bytePos = p;
    return *this;
}

DUTF8String::iterator DUTF8String::iterator::operator--(int) {
    iterator tmp = *this;
    --(*this);
    return tmp;
}

DUTF8String::iterator& DUTF8String::iterator::operator+=(difference_type n) {
    if (!str) return *this;
    if (n > 0) {
        while (n-- > 0) ++(*this);
    } else {
        while (n++ < 0) --(*this);
    }
    return *this;
}

DUTF8String::iterator& DUTF8String::iterator::operator-=(difference_type n) {
    return (*this += -n);
}

DUTF8String::iterator DUTF8String::iterator::operator+(difference_type n) const {
    iterator tmp = *this;
    tmp += n;
    return tmp;
}

DUTF8String::iterator DUTF8String::iterator::operator-(difference_type n) const {
    iterator tmp = *this;
    tmp -= n;
    return tmp;
}

DUTF8String::iterator::difference_type DUTF8String::iterator::operator-(const iterator& other) const {
    if (str != other.str) throw std::runtime_error("iterator::operator- on different containers");
    // compute codepoint distance by scanning between byte positions
    if (bytePos == other.bytePos) return 0;
    if (other.bytePos < bytePos) {
        // scan forward from other to this
        difference_type dist = 0;
        iterator tmp = other;
        while (tmp.bytePos < bytePos) { ++tmp; ++dist; }
        return dist;
    } else {
        difference_type dist = 0;
        iterator tmp = *this;
        while (tmp.bytePos < other.bytePos) { ++tmp; --dist; }
        return dist;
    }
}

bool DUTF8String::iterator::operator==(const iterator& o) const {
    return str == o.str && bytePos == o.bytePos;
}
bool DUTF8String::iterator::operator!=(const iterator& o) const { return !(*this == o); }

bool DUTF8String::iterator::operator<(const iterator& o)  const { return (str == o.str) ? bytePos < o.bytePos : false; }
bool DUTF8String::iterator::operator>(const iterator& o)  const { return (str == o.str) ? bytePos > o.bytePos : false; }
bool DUTF8String::iterator::operator<=(const iterator& o) const { return (str == o.str) ? bytePos <= o.bytePos : false; }
bool DUTF8String::iterator::operator>=(const iterator& o) const { return (str == o.str) ? bytePos >= o.bytePos : false; }

// ----------------------------- reverse iterator helpers -----------------------------

DUTF8String::iterator DUTF8String::begin() const { return iterator(&data, 0); }
DUTF8String::iterator DUTF8String::end()   const { return iterator(&data, data.size()); }
DUTF8String::reverse_iterator DUTF8String::rbegin() const { return reverse_iterator(end()); }
DUTF8String::reverse_iterator DUTF8String::rend()   const { return reverse_iterator(begin()); }

// ----------------------------- length / cache -----------------------------

size_t DUTF8String::length() const {
    if (cache_valid) return cached_len;
    size_t cnt = 0;
    for (auto it = begin(); it != end(); ++it) ++cnt;
    cached_len = cnt;
    cache_valid = true;
    return cached_len;
}

bool DUTF8String::empty() const noexcept { return data.empty(); }
void DUTF8String::clear() { data.clear(); invalidate_cache(); }

std::string DUTF8String::to_string() const { return data; }
std::string_view DUTF8String::to_string_view() const noexcept { return std::string_view(data); }
const char* DUTF8String::c_str() const noexcept { return data.c_str(); }

// ----------------------------- operator[] (codepoint index) -----------------------------

char32_t DUTF8String::operator[](size_t index) const {
    auto it = begin();
    for (size_t i = 0; i < index && it != end(); ++i) ++it;
    if (it == end()) throw std::out_of_range("DUTF8String::operator[] out of range");
    return *it;
}

// ----------------------------- append/insert/erase codepoints -----------------------------

void DUTF8String::append_codepoint(char32_t cp) {
    encode_utf8(cp, data);
    invalidate_cache();
}

void DUTF8String::insert_codepoint(size_t index, char32_t cp) {
    // find byte pos for codepoint index
    size_t bytePos = 0;
    size_t cur = 0;
    while (bytePos < data.size() && cur < index) {
        unsigned char c = static_cast<unsigned char>(data[bytePos]);
        bytePos += utf8_char_length(c);
        ++cur;
    }
    std::string tmp;
    encode_utf8(cp, tmp);
    data.insert(bytePos, tmp);
    invalidate_cache();
}

void DUTF8String::erase_codepoint(size_t index) {
    size_t bytePos = 0;
    size_t cur = 0;
    while (bytePos < data.size() && cur < index) {
        unsigned char c = static_cast<unsigned char>(data[bytePos]);
        bytePos += utf8_char_length(c);
        ++cur;
    }
    if (bytePos >= data.size()) return;
    size_t next = bytePos + utf8_char_length(static_cast<unsigned char>(data[bytePos]));
    data.erase(bytePos, next - bytePos);
    invalidate_cache();
}

// erase by codepoint-range
DUTF8String& DUTF8String::erase(size_t pos_cp, size_t count_cp) {
    // find start byte iterator for pos_cp
    size_t bytePos = 0;
    size_t cur = 0;
    while (bytePos < data.size() && cur < pos_cp) {
        unsigned char c = static_cast<unsigned char>(data[bytePos]);
        bytePos += utf8_char_length(c);
        ++cur;
    }
    if (cur < pos_cp) throw std::out_of_range("DUTF8String::erase pos out of range");

    if (count_cp == SIZE_MAX) {
        data.erase(bytePos);
        invalidate_cache();
        return *this;
    }

    // find end byte position after count_cp codepoints
    size_t tmp = bytePos;
    size_t removed = 0;
    while (tmp < data.size() && removed < count_cp) {
        unsigned char c = static_cast<unsigned char>(data[tmp]);
        tmp += utf8_char_length(c);
        ++removed;
    }
    data.erase(bytePos, tmp - bytePos);
    invalidate_cache();
    return *this;
}

// erase by iterator range
DUTF8String& DUTF8String::erase(iterator it_start, iterator it_end) {
    if (it_start.container() != &data || it_end.container() != &data) {
        throw std::runtime_error("DUTF8String::erase: iterators from different containers");
    }
    size_t byte_start = it_start.byte_index();
    size_t byte_end   = it_end.byte_index();
    if (byte_start > byte_end) std::swap(byte_start, byte_end);
    if (byte_start > data.size() || byte_end > data.size()) throw std::out_of_range("DUTF8String::erase: iterator out of range");
    data.erase(byte_start, byte_end - byte_start);
    invalidate_cache();
    return *this;
}

// ----------------------------- substr (codepoint based) -----------------------------

std::string DUTF8String::substr(size_t start_cp, size_t count_cp) const {
    auto it = data.cbegin();
    auto endit = data.cend();
    std::string out;
    size_t index = 0;
    while (it != endit) {
        auto cur = it;
        size_t next;
        char32_t cp = decode_utf8_at(data, std::distance(data.cbegin(), cur), next); // not ideal: fix below
        // The above used std::distance incorrectly for string::const_iterator; we'll instead use manual byte cursor
        break;
    }
    // Implement simpler: manual byte cursor
    size_t bytePos = 0;
    size_t cur = 0;
    // skip until start_cp
    while (bytePos < data.size() && cur < start_cp) {
        unsigned char c = static_cast<unsigned char>(data[bytePos]);
        bytePos += utf8_char_length(c);
        ++cur;
    }
    // collect count_cp codepoints
    size_t collected = 0;
    while (bytePos < data.size() && collected < count_cp) {
        size_t next = bytePos + utf8_char_length(static_cast<unsigned char>(data[bytePos]));
        out.append(data, bytePos, next - bytePos);
        bytePos = next;
        ++collected;
    }
    return out;
}

// ----------------------------- find (single codepoint or substring) -----------------------------

size_t DUTF8String::find(char32_t target, size_t start_cp) const {
    size_t bytePos = 0;
    size_t cur = 0;
    // move to start_cp
    while (bytePos < data.size() && cur < start_cp) {
        unsigned char c = static_cast<unsigned char>(data[bytePos]);
        bytePos += utf8_char_length(c);
        ++cur;
    }
    // scan
    while (bytePos < data.size()) {
        size_t next = 0;
        char32_t cp = decode_utf8_at(data, bytePos, next);
        if (cp == target) return cur;
        bytePos = next;
        ++cur;
    }
    return SIZE_MAX;
}

size_t DUTF8String::find(const std::string& sub, size_t start_cp) const {
    if (sub.empty()) return 0;
    // decode pattern into codepoints
    std::vector<char32_t> pattern;
    size_t ppos = 0;
    while (ppos < sub.size()) {
        size_t np = 0;
        char32_t cp = decode_utf8_at(sub, ppos, np);
        pattern.push_back(cp);
        ppos = np;
    }
    // scan source
    size_t bytePos = 0;
    size_t cur = 0;
    // move to start_cp
    while (bytePos < data.size() && cur < start_cp) {
        unsigned char c = static_cast<unsigned char>(data[bytePos]);
        bytePos += utf8_char_length(c);
        ++cur;
    }
    while (bytePos < data.size()) {
        size_t tmp = bytePos;
        size_t i = 0;
        bool ok = true;
        while (i < pattern.size()) {
            if (tmp >= data.size()) { ok = false; break; }
            size_t np = 0;
            char32_t cp = decode_utf8_at(data, tmp, np);
            if (cp != pattern[i]) { ok = false; break; }
            tmp = np;
            ++i;
        }
        if (ok) return cur;
        // advance one codepoint
        size_t np = 0;
        decode_utf8_at(data, bytePos, np);
        bytePos = np;
        ++cur;
    }
    return SIZE_MAX;
}

// ----------------------------- replace -----------------------------

void DUTF8String::replace(char32_t target, char32_t replacement) {
    std::string out;
    size_t bytePos = 0;
    while (bytePos < data.size()) {
        size_t next = 0;
        char32_t cp = decode_utf8_at(data, bytePos, next);
        if (cp == target) encode_utf8(replacement, out);
        else out.append(data, bytePos, next - bytePos);
        bytePos = next;
    }
    data.swap(out);
    invalidate_cache();
}

void DUTF8String::replace(const std::string& search, const std::string& replace_with) {
    if (search.empty()) return;
    // pattern as codepoints
    std::vector<char32_t> pattern;
    size_t ppos = 0;
    while (ppos < search.size()) {
        size_t np = 0;
        pattern.push_back(decode_utf8_at(search, ppos, np));
        ppos = np;
    }
    std::string out;
    size_t bytePos = 0;
    while (bytePos < data.size()) {
        size_t tmp = bytePos;
        size_t i = 0;
        bool ok = true;
        while (i < pattern.size()) {
            if (tmp >= data.size()) { ok = false; break; }
            size_t np = 0;
            char32_t cp = decode_utf8_at(data, tmp, np);
            if (cp != pattern[i]) { ok = false; break; }
            tmp = np;
            ++i;
        }
        if (ok) {
            out += replace_with;
            bytePos = tmp;
        } else {
            size_t np = 0;
            char32_t cp = decode_utf8_at(data, bytePos, np);
            encode_utf8(cp, out);
            bytePos = np;
        }
    }
    data.swap(out);
    invalidate_cache();
}

// ----------------------------- UTF-32 conversions -----------------------------

std::u32string DUTF8String::to_utf32() const {
    std::u32string out;
    size_t bytePos = 0;
    while (bytePos < data.size()) {
        size_t np = 0;
        char32_t cp = decode_utf8_at(data, bytePos, np);
        out.push_back(cp);
        bytePos = np;
    }
    return out;
}

// ----------------------------- non-member concatenation -----------------------------

DUTF8String operator+(const DUTF8String& lhs, const DUTF8String& rhs) {
    DUTF8String tmp(lhs);
    tmp.data += rhs.data;
    tmp.invalidate_cache();
    return tmp;
}

DUTF8String operator+(const std::string& lhs, const DUTF8String& rhs) {
    DUTF8String tmp(lhs);
    tmp.data += rhs.data;
    tmp.invalidate_cache();
    return tmp;
}

DUTF8String operator+(const char* lhs, const DUTF8String& rhs) {
    DUTF8String tmp(lhs);
    tmp.data += rhs.data;
    tmp.invalidate_cache();
    return tmp;
}

} // namespace DTools
