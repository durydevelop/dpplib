/**
 * DUTF8String.cpp
 * DUTF8String.h
 * 
 * String class to handle Unicode strings like std::string. You can use it like a normal std::string with add-on methods to hamdle "UTF8 bytecodes".
 * Features:
 * - Most of std::string operators and methods has been impemented.
 * - Conversion from and to std::string, std::string_view, c_str().
 * - Conversion from utf8 to utf32 and vice-versa.
 * - Assign operators and methods for std::string and *char.
 * - Manipulation methods for codepoints.
 * - KMP-based search/replace methods.
 * - Iterator and reverse iterator.
 * 
 * N.B. All ...pos... variables in this class are indexes.
 */

#include "dpplib/DUTF8String.h"
#include <algorithm>

namespace DTools {
// *****************************************************************************************
// *********************************** DUTF8String class ***********************************
// *****************************************************************************************
    DUTF8String::DUTF8String(const std::string& utf8) : data(utf8) {
        invalidate_cache();
    }

    DUTF8String::DUTF8String(std::string&& utf8) noexcept : data(std::move(utf8)) {
        invalidate_cache();
    }

    DUTF8String::DUTF8String(std::string_view utf8) : data(utf8.begin(), utf8.end()){
    }

    DUTF8String::DUTF8String(const char* utf8) : data(utf8 ? utf8 : "") {
        invalidate_cache();
    }

    DUTF8String::DUTF8String(const std::vector<char32_t>& codepoints) {
        for (char32_t codepoint : codepoints) encode_utf8(codepoint, data);
        invalidate_cache();
    }

    DUTF8String::DUTF8String(const std::u32string& utf32) {
        for (char32_t codepoint : utf32) encode_utf8(codepoint, data);
        invalidate_cache();
    }

// -------------------------------------- Operators --------------------------------------
    DUTF8String& DUTF8String::operator=(const std::string& s) {
        data=s;
        invalidate_cache();
        return *this;
    }

    DUTF8String& DUTF8String::operator=(std::string_view s) {
        data.assign(s.begin(), s.end());
        invalidate_cache();
        return *this;
    }

    DUTF8String& DUTF8String::operator=(std::string&& s) noexcept {
        data=std::move(s);
        invalidate_cache();
        return *this;
    }

    DUTF8String& DUTF8String::operator=(const char* s) {
        data=s ? s : "";
        invalidate_cache();
        return *this;
    }

    bool DUTF8String::operator==(const std::string& s) const {
        return data == s;
    }

    bool DUTF8String::operator==(std::string_view sv) const {
        return data == sv;
    }

    bool DUTF8String::operator==(const char* s) const {
        return data == s;
    }

    bool DUTF8String::iterator::operator==(const iterator& o) const {
        return str == o.str && bytePos == o.bytePos;
    }

    bool DUTF8String::iterator::operator!=(const iterator& o) const {
        return !(*this == o);
    }

    bool DUTF8String::iterator::operator<(const iterator& o)  const {
        return (str == o.str) ? bytePos < o.bytePos : false;
    }

    bool DUTF8String::iterator::operator>(const iterator& o)  const {
        return (str == o.str) ? bytePos > o.bytePos : false;
    }

    bool DUTF8String::iterator::operator<=(const iterator& o) const {
        return (str == o.str) ? bytePos <= o.bytePos : false;
    }

    bool DUTF8String::iterator::operator>=(const iterator& o) const {
        return (str == o.str) ? bytePos >= o.bytePos : false;
    }

    DUTF8String& DUTF8String::operator+=(const DUTF8String& other) {
        data+=other.data;
        invalidate_cache();
        return *this;
    }

    DUTF8String& DUTF8String::operator+=(const std::string& s) {
        data+=s;
        invalidate_cache();
        return *this;
    }

    DUTF8String& DUTF8String::operator+=(const char* s) {
        if (s) data.append(s);
        invalidate_cache();
        return *this;
    }

    DUTF8String& DUTF8String::operator+=(char32_t codepoint) {
        encode_utf8(codepoint, data);
        invalidate_cache();
        return *this;
    }

    char32_t DUTF8String::operator[](size_t index) const {
        auto it=begin();
        for (size_t i=0; i < index && it != end(); ++i) {
            ++it;
        }
        if (it == end()) {
            throw std::out_of_range("DUTF8String::operator[] out of range");
        }
        return *it;
    }

// --------------------------------- Conversions -------------------------------------
    std::string DUTF8String::to_string() const {
        return data;
    }

    std::string_view DUTF8String::to_string_view() const noexcept {
        return std::string_view(data);
    }

    const char* DUTF8String::c_str() const noexcept {
        return data.c_str();
    }

    std::u32string DUTF8String::to_utf32() const {
        std::u32string out;
        size_t bytePos=0;
        while (bytePos < data.size()) {
            size_t next=0;
            char32_t codepoint=decode_utf8_at(data, bytePos, next);
            out.push_back(codepoint);
            bytePos=next;
        }
        return out;
    }

    std::string DUTF8String::utf32_to_utf8(const std::u32string& src) {
        std::string out;
        out.reserve(src.size() * 3);
        for (char32_t codepoint : src) {
            encode_utf8(codepoint, out);
        }
        return out;
    }

    std::u32string DUTF8String::utf8_to_utf32(const std::string& src) {
        std::u32string out;
        size_t p=0;
        while (p < src.size()) {
            size_t next=0;
            char32_t codepoint=decode_utf8_at(src, p, next);
            out.push_back(codepoint);
            p=next;
        }
        return out;
    }

// -------------------------------- Modifications ------------------------------------
    DUTF8String& DUTF8String::assign(const DUTF8String& other) {
        data=other.data;
        return *this;
    }

    DUTF8String& DUTF8String::assign(DUTF8String&& other) noexcept {
        data=std::move(other.data);
        return *this;
    }

    DUTF8String& DUTF8String::assign(const std::string& s) {
        data.assign(s);
        return *this;
    }

    DUTF8String& DUTF8String::assign(std::string&& s) noexcept {
        data=std::move(s);
        return *this;
    }

    DUTF8String& DUTF8String::assign(const char* s) {
        data.assign(s ? s : "");
        return *this;
    }

    DUTF8String& DUTF8String::assign(const char* s, size_t count) {
        data.assign(s, count);
        return *this;
    }

    /**
     * 
     * @returns byte index of the appended code point
     */
    size_t DUTF8String::append_codepoint(char32_t codepoint) {
        size_t ret=data.size();
        encode_utf8(codepoint, data);
        invalidate_cache();
        return ret;
    }

    void DUTF8String::insert_codepoint(size_t index, char32_t codepoint) {
        size_t bytePos=codepoint_index_to_byte_index(index);
        std::string tmp;
        encode_utf8(codepoint, tmp);
        data.insert(bytePos, tmp);
        invalidate_cache();
    }

    // TODO: test
    void DUTF8String::insert(size_t index, const std::string& str) {
        size_t bytePos=codepoint_index_to_byte_index(index);
        data.insert(bytePos, str);
        invalidate_cache();
    }

    void DUTF8String::erase_codepoint(size_t index) {
        size_t bytePos=codepoint_index_to_byte_index(index);
        if (bytePos >= data.size()) {
            return;
        }
        size_t next=bytePos + utf8_char_length(static_cast<unsigned char>(data[bytePos]));
        data.erase(bytePos, next - bytePos);
        invalidate_cache();
    }

    DUTF8String& DUTF8String::erase(size_t index_cp, size_t count_cp) {
        size_t bytePos=codepoint_index_to_byte_index(index_cp);
        if (bytePos > data.size()) {
            throw std::out_of_range("DUTF8String::erase pos out of range");
        }
        if (count_cp == SIZE_MAX) {
            data.erase(bytePos);
            invalidate_cache();
            return *this;
        }
        size_t tmp=bytePos;
        size_t removed=0;
        while (tmp < data.size() && removed < count_cp) {
            tmp+=utf8_char_length(static_cast<unsigned char>(data[tmp]));
            ++removed;
        }
        data.erase(bytePos, tmp - bytePos);
        invalidate_cache();
        return *this;
    }

    DUTF8String& DUTF8String::erase(iterator it_start, iterator it_end) {
        if (it_start.container() != &data || it_end.container() != &data) {
            throw std::runtime_error("DUTF8String::erase: iterators from different containers");
        }
        size_t byte_start=it_start.byte_index();
        size_t byte_end  =it_end.byte_index();
        if (byte_start > byte_end) {
            std::swap(byte_start, byte_end);
        }
        if (byte_start > data.size() || byte_end > data.size()) {
            throw std::out_of_range("DUTF8String::erase: iterator out of range");
        }
        data.erase(byte_start, byte_end - byte_start);
        invalidate_cache();
        return *this;
    }

    void DUTF8String::replace_codepoint(char32_t target, char32_t replacement) {
        std::string out;
        size_t bytePos=0;
        while (bytePos < data.size()) {
            size_t next=0;
            char32_t codepoint=decode_utf8_at(data, bytePos, next);
            if (codepoint == target) encode_utf8(replacement, out);
            else out.append(data, bytePos, next - bytePos);
            bytePos=next;
        }
        data.swap(out);
        invalidate_cache();
    }

    void DUTF8String::replace(const std::string& search, const std::string& replace_with_naive) {
        // fallback naive replace (keeps existing behaviour)
        if (search.empty()) {
            return;
        }
        std::u32string pattern=utf8_to_utf32(search);
        std::u32string repl=utf8_to_utf32(replace_with_naive);
        std::u32string txt=to_utf32();
        std::u32string out;
        out.reserve(txt.size());
        size_t i=0;
        while (i < txt.size()) {
            bool matched=false;
            if (i + pattern.size() <= txt.size()) {
                matched=true;
                for (size_t j=0; j < pattern.size(); ++j) {
                    if (txt[i+j] != pattern[j]) {
                        matched=false; break;
                    }
                }
            }
            if (matched) {
                for (char32_t codepoint : repl) {
                    out.push_back(codepoint);
                }
                i+=pattern.size();
            }
            else {
                out.push_back(txt[i]);
                ++i;
            }
        }
        data=utf32_to_utf8(out);
        invalidate_cache();
    }

    void DUTF8String::pop_back() {
        if (data.empty()) {
            return;
        }

        auto it=data.end();
        
        // To start of last codepoint
        do {
            --it;
            unsigned char c=static_cast<unsigned char>(*it);
            // i byte di continuazione UTF-8 hanno il formato 10xxxxxx (0x80..0xBF)
        } while ((static_cast<unsigned char>(*it) & 0xC0) == 0x80 && it != data.begin());

        data.erase(it, data.end());
    }

// ------------------------------- Interrogations ------------------------------------
    std::string DUTF8String::substr(size_t start_cp, size_t count_cp) const {
        std::string out;
        size_t bytePos=codepoint_index_to_byte_index(start_cp);
        size_t collected=0;
        while (bytePos < data.size() && collected < count_cp) {
            size_t next=bytePos + utf8_char_length(static_cast<unsigned char>(data[bytePos]));
            out.append(data, bytePos, next - bytePos);
            bytePos=next;
            ++collected;
        }
        return out;
    }

    std::string_view DUTF8String::substr_view(size_t start_cp, size_t count_cp) const {
        size_t start_byte=codepoint_index_to_byte_index(start_cp);
        size_t byte=start_byte;

        size_t collected=0;
        while (byte < data.size() && collected < count_cp) {
            byte+=utf8_char_length(static_cast<unsigned char>(data[byte]));
            ++collected;
        }

        size_t end_byte=byte;
        return std::string_view{data.data() + start_byte, end_byte - start_byte};
    }

    size_t DUTF8String::find(char32_t target, size_t start_cp) const {
        size_t bytePos=codepoint_index_to_byte_index(start_cp);
        size_t cur=start_cp;
        while (bytePos < data.size()) {
            size_t next=0;
            char32_t cp=decode_utf8_at(data, bytePos, next);
            if (cp == target) {
                return cur;
            }
            bytePos=next;
            ++cur;
        }
        return SIZE_MAX;
    }

    size_t DUTF8String::find(const std::string& sub, size_t start_cp) const {
        // naive (keeps existing behaviour)
        if (sub.empty()) {
            return 0;
        }
        std::u32string pat=utf8_to_utf32(sub);
        std::u32string txt=to_utf32();
        if (start_cp >= txt.size()) {
            return SIZE_MAX;
        }
        // simple scan
        for (size_t i=start_cp; i + pat.size() <= txt.size(); ++i) {
            bool ok=true;
            for (size_t j=0; j < pat.size(); ++j) {
                if (txt[i+j] != pat[j]) {
                    ok=false; break;
                }
            }
            if (ok) {
                return i;
            }
        }
        return SIZE_MAX;
    }

    bool DUTF8String::empty() const noexcept {
        return data.empty();
    }

    void DUTF8String::clear() {
        data.clear(); invalidate_cache();
    }

    size_t DUTF8String::length() const {
        if (cache_valid) {
            return cached_len;
        }
        size_t cnt=0;
        for (auto it=begin(); it != end(); ++it) {
            ++cnt;
        }
        cached_len=cnt;
        cache_valid=true;
        return cached_len;
    }

    size_t DUTF8String::codepoint_index_to_byte_index(size_t index_cp) const {
        size_t bytePos=0;
        size_t cur=0;
        while (bytePos < data.size() && cur < index_cp) {
            unsigned char c=static_cast<unsigned char>(data[bytePos]);
            bytePos+=utf8_char_length(c);
            ++cur;
        }
        return bytePos;
    }

    void DUTF8String::generate_cp_index_list(std::vector<size_t>& destList) {
        //destList.emplace_back(0);
        for (auto it=begin(); it != end(); ++it) {
            destList.emplace_back(it.byte_index());
        }
    }

// --------------------- KMP-based search/replace on codepoints ----------------------
    size_t DUTF8String::find_kmp(const std::string& sub, size_t start_cp) const {
        if (sub.empty()) {
            return 0;
        }
        std::u32string pat=utf8_to_utf32(sub);
        std::u32string txt=to_utf32();
        if (start_cp >= txt.size()) {
            return SIZE_MAX;
        }
        std::vector<int> lps=build_lps(pat);
        size_t i=start_cp;
        size_t j=0;
        while (i < txt.size()) {
            if (txt[i] == pat[j]) {
                ++i;
                ++j;
                if (j == pat.size()) {
                    return i - j;
                }
            }
            else {
                if (j > 0) {
                    j=lps[j-1];
                }
                else {
                    ++i;
                }
            }
        }
        return SIZE_MAX;
    }

    void DUTF8String::replace_kmp(const std::string& search, const std::string& replace_with) {
        if (search.empty()) {
            return;
        }
        std::u32string pat=utf8_to_utf32(search);
        std::u32string repl=utf8_to_utf32(replace_with);
        std::u32string txt=to_utf32();

        std::vector<int> lps=build_lps(pat);
        std::u32string out;
        out.reserve(txt.size());

        size_t i=0;
        size_t j=0;
        size_t last_copy=0; // index in txt of next segment to copy

        while (i < txt.size()) {
            if (txt[i] == pat[j]) {
                ++i; ++j;
                if (j == pat.size()) {
                    // copy chunk from last_copy .. (i - j)
                    for (size_t k=last_copy; k < i - j; ++k) {
                        out.push_back(txt[k]);
                    }
                    // append replacement
                    for (char32_t codepoint : repl) {
                        out.push_back(codepoint);
                    }
                    last_copy=i; // continue after matched segment
                    j=lps[j-1];
                }
            }
            else {
                if (j > 0) {
                    j=lps[j-1];
                }
                else {
                    ++i;
                }
            }
        }
        // copy tail
        for (size_t k=last_copy; k < txt.size(); ++k) {
            out.push_back(txt[k]);
        }

        // convert back to utf8
        data=utf32_to_utf8(out);
        invalidate_cache();
    }

// ------------------------------------- Iterators -----------------------------------
    DUTF8String::iterator DUTF8String::begin() const {
        return iterator(&data, 0);
    }

    DUTF8String::iterator DUTF8String::end() const {
        return iterator(&data, data.size());
    }

    DUTF8String::reverse_iterator DUTF8String::rbegin() const {
        return reverse_iterator(end());
    }

    DUTF8String::reverse_iterator DUTF8String::rend() const {
        return reverse_iterator(begin());
    }

// -----------------------------------------------------------------------------------
// --------------------------------- Privates members --------------------------------
// -----------------------------------------------------------------------------------
// ---------------------------------- Static helpers ---------------------------------
    void DUTF8String::invalidate_cache() const {
        cache_valid=false;
    }

    /**
     * Get lenght of the codepoint from first byte
     */
    size_t DUTF8String::utf8_char_length(unsigned char c) noexcept {
        if ((c & 0x80) == 0x00) return 1; // start with 0(ASCII) -> 1 byte
        if ((c & 0xE0) == 0xC0) return 2; // start with 110xxxxx -> 2 bytes
        if ((c & 0xF0) == 0xE0) return 3; // start with 1110xxxx -> 3 bytes
        if ((c & 0xF8) == 0xF0) return 4; // start with 11110xxx -> 4 bytes
        return 1;
    }

    char32_t DUTF8String::decode_utf8_at(const std::string& s, size_t p, size_t &next_p) {
        next_p=p;
        if (p >= s.size()) {
            return 0;
        }
        const unsigned char* ptr=reinterpret_cast<const unsigned char*>(s.data() + p);
        unsigned char c0=ptr[0];
        if ((c0 & 0x80) == 0x00) {
            next_p=p + 1; return c0;
        }
        if ((c0 & 0xE0) == 0xC0) {
            if (p + 1 >= s.size()) {
                throw std::runtime_error("Truncated UTF-8");
            }
            next_p=p + 2;
            return ((ptr[0] & 0x1F) << 6) | (ptr[1] & 0x3F);
        }
        if ((c0 & 0xF0) == 0xE0) {
            if (p + 2 >= s.size()) {
                throw std::runtime_error("Truncated UTF-8");
            }
            next_p=p + 3;
            return ((ptr[0] & 0x0F) << 12) | ((ptr[1] & 0x3F) << 6) | (ptr[2] & 0x3F);
        }
        if ((c0 & 0xF8) == 0xF0) {
            if (p + 3 >= s.size()) {
                throw std::runtime_error("Truncated UTF-8");
            }
            next_p=p + 4;
            return ((ptr[0] & 0x07) << 18) | ((ptr[1] & 0x3F) << 12) | ((ptr[2] & 0x3F) << 6) | (ptr[3] & 0x3F);
        }
        throw std::runtime_error("Invalid UTF-8 sequence");
    }

    void DUTF8String::encode_utf8(char32_t codepoint, std::string& out) {
        if (codepoint <= 0x7F) {
            out.push_back(static_cast<char>(codepoint));
        }
        else if (codepoint <= 0x7FF) {
            out.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
            out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
        else if (codepoint <= 0xFFFF) {
            out.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
            out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
        else {
            out.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
            out.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
    }

    void DUTF8String::move_prev_utf8(const std::string& s, size_t& index) {
        if (index == 0) {
            return;
        }
        size_t i=index - 1;
        while (i > 0 && (static_cast<unsigned char>(s[i]) & 0xC0) == 0x80) {
            --i;
        }
        index=i;
    }

    std::vector<int> DUTF8String::build_lps(const std::u32string& pat) {
        size_t m=pat.size();
        std::vector<int> lps(m, 0);
        int len=0;
        for (size_t i=1; i < m; ++i) {
            while (len > 0 && pat[i] != pat[len]) {
                len=lps[len-1];
            }
            if (pat[i] == pat[len]) {
                ++len;
            }
            lps[i]=len;
        }
        return lps;
    }

    bool DUTF8String::is_unicode(int codepoint) {
        return (codepoint > 0 && codepoint <= 0x10FFFF && !(codepoint >= 0xD800 && codepoint <= 0xDFFF));
    }

// ----------------------------- non-member concatenation -----------------------------

    DUTF8String operator+(const DUTF8String& lhs, const DUTF8String& rhs) {
        DUTF8String tmp(lhs);
        tmp+=rhs;
        return tmp;
    }

    DUTF8String operator+(const std::string& lhs, const DUTF8String& rhs) {
        DUTF8String tmp(lhs);
        tmp+=rhs;
        return tmp;
    }

    DUTF8String operator+(const char* lhs, const DUTF8String& rhs) {
        DUTF8String tmp(lhs);
        tmp+=rhs;
        return tmp;
    }

// *****************************************************************************************
// ************************************* iterator class ************************************
// *****************************************************************************************

    DUTF8String::iterator::iterator() : str(nullptr), bytePos(0) {

    }
    
    DUTF8String::iterator::iterator(const std::string* s, size_t index) : str(s), bytePos(index) {

    }

// -------------------------------------- Operators --------------------------------------
    DUTF8String::iterator::difference_type DUTF8String::iterator::operator-(const iterator& other) const {
        if (str != other.str) {
            throw std::runtime_error("iterator::operator- on different containers");
        }
        if (bytePos == other.bytePos) {
            return 0;
        }
        if (other.bytePos < bytePos) {
            difference_type dist=0;
            iterator tmp=other;
            while (tmp.bytePos < bytePos) {
                ++tmp;
                ++dist;
            }
            return dist;
        }
        else {
            difference_type dist=0;
            iterator tmp=*this;
            while (tmp.bytePos < other.bytePos) {
                ++tmp;
                --dist;
            }
            return dist;
        }
    }

    char32_t DUTF8String::iterator::operator*() const {
        if (!str) {
            return 0;
        }
        size_t np=bytePos;
        return peek_codepoint_at(*str, bytePos, np);
    }

    DUTF8String::iterator& DUTF8String::iterator::operator++() {
        if (!str) {
            return *this;
        }
        if (bytePos >= str->size()) {
            return *this;
        }
        unsigned char c=static_cast<unsigned char>((*str)[bytePos]);
        bytePos+=DUTF8String::utf8_char_length(c);
        return *this;
    }

    DUTF8String::iterator DUTF8String::iterator::operator++(int) {
        iterator tmp=*this;
        ++(*this);
        return tmp;
    }

    DUTF8String::iterator& DUTF8String::iterator::operator--() {
        if (!str) {
            return *this;
        }
        if (bytePos == 0) {
            return *this;
        }
        size_t p=bytePos - 1;
        while (p > 0 && (static_cast<unsigned char>((*str)[p]) & 0xC0) == 0x80) {
            --p;
        }
        bytePos=p;
        return *this;
    }

    DUTF8String::iterator DUTF8String::iterator::operator--(int) {
        iterator tmp=*this;
        --(*this);
        return tmp;
    }

    DUTF8String::iterator& DUTF8String::iterator::operator+=(difference_type n) {
        if (!str) {
            return *this;
        }
        if (n > 0) {
            while (n-- > 0) {
                ++(*this);
            }
        }
        else {
            while (n++ < 0) {
                --(*this);
            }
        }
        return *this;
    }

    DUTF8String::iterator& DUTF8String::iterator::operator-=(difference_type n) {
        return (*this+=-n);
    }

    DUTF8String::iterator DUTF8String::iterator::operator+(difference_type n) const {
        iterator tmp=*this;
        tmp+=n;
        return tmp;
    }

    DUTF8String::iterator DUTF8String::iterator::operator-(difference_type n) const {
        iterator tmp=*this;
        tmp-=n;
        return tmp;
    }

    size_t DUTF8String::iterator::byte_index() const {
        return bytePos;
    }
    const std::string* DUTF8String::iterator::container() const {
        return str;
    }
// ----------------------------- iterator helpers -----------------------------

    char32_t DUTF8String::iterator::peek_codepoint_at(const std::string& str, size_t p, size_t &next_p) {
        return DUTF8String::decode_utf8_at(str, p, next_p);
    }

// ***********************************************************************************
// ********************************* reverse_iterator ********************************
// ***********************************************************************************

    DUTF8String::reverse_iterator::reverse_iterator() : it() {

    }

    DUTF8String::reverse_iterator::reverse_iterator(const iterator& i) : it(i) {

    }

    char32_t DUTF8String::reverse_iterator::operator*() const {
        iterator tmp=it;
        --tmp;
        return *tmp;
    }

    DUTF8String::reverse_iterator& DUTF8String::reverse_iterator::operator++() {
        --it;
        return *this;
    }

    DUTF8String::reverse_iterator DUTF8String::reverse_iterator::operator++(int) {
        reverse_iterator tmp=*this; ++(*this);
        return tmp;
    }

    DUTF8String::reverse_iterator& DUTF8String::reverse_iterator::operator--() {
        ++it; return *this;
    }

    DUTF8String::reverse_iterator DUTF8String::reverse_iterator::operator--(int) {
        reverse_iterator tmp=*this;
        --(*this);
        return tmp;
    }

    bool DUTF8String::reverse_iterator::operator==(const reverse_iterator& o) const {
        return it == o.it;
    }

    bool DUTF8String::reverse_iterator::operator!=(const reverse_iterator& o) const {
        return it != o.it;
    }
}
