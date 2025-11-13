#include "dpplib/DUTF8String.h"


#include <charconv>
#include <stdexcept>
#include <limits>

namespace DTools
{

    DUTF8String::DUTF8String(const std::vector<char32_t>& cps) {
        for (char32_t cp : cps) {
            encode_utf8(cp, data);
        }
    }

    // ============================================================
    // Codifica un codepoint in UTF-8
    // ============================================================
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
        } else if (cp <= 0x10FFFF) {
            out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else {
            throw std::runtime_error("Invalid Unicode codepoint");
        }
    }

    // ============================================================
    // Conversioni base
    // ============================================================
    std::string DUTF8String::to_string() {
        return std::string(reinterpret_cast<const char*>(data.data()), data.size());
    }

    std::string_view DUTF8String::to_string_view(void) noexcept {
        return std::string_view(reinterpret_cast<const char*>(data.data()), data.size());
    }

    // ============================================================
    // Conta i codepoints in una stringa UTF-8
    // ============================================================
    size_t DUTF8String::length(void) {
        auto it = data.cbegin();
        size_t count = 0;
        while (it != data.end()) {
            next_codepoint(it, data.cend());
            ++count;
        }
        return count;
    }

    // ============================================================
    // Estrae una sottostringa per codepoint
    // ============================================================
    std::string DUTF8String::substr(size_t start_cp, size_t count_cp) {
        auto it = data.cbegin();
        auto end = data.cend();
        std::string out;
        size_t index = 0;

        while (it != end) {
            auto current = it;
            char32_t cp = next_codepoint(it, end);
            if (index >= start_cp && (index - start_cp) < count_cp)
                out.append(current, it);
            if ((index - start_cp) >= count_cp) break;
            ++index;
        }
        return out;
    }

    // ============================================================
    // Trova un singolo codepoint
    // ============================================================
    inline size_t DUTF8String::find(char32_t target, size_t start_cp) {
        auto it = data.cbegin();
        auto end = data.cend();
        size_t index = 0;
        while (it != end && index < start_cp) {
            next_codepoint(it, end);
            ++index;
        }
        while (it != end) {
            char32_t cp = next_codepoint(it, end);
            if (cp == target) {
                return index;
            }
            ++index;
        }
        return SIZE_MAX;
    }

    // ============================================================
    // Trova una sottostringa UTF-8
    // ============================================================
    size_t DUTF8String::find(const std::string& sub, size_t start_cp) {
        if (sub.empty()) return 0;
        std::vector<char32_t> pattern;
        auto pit = sub.cbegin();
        while (pit != sub.cend()) {
            pattern.push_back(next_codepoint(pit, sub.end()));
        }
        auto it = data.cbegin();
        auto end = data.cend();
        size_t index = 0;
        while (it != end && index < start_cp) {
            next_codepoint(it, end);
            ++index;
        }
        while (it != end) {
            auto cur = it;
            auto tmp = it;
            size_t i = 0;
            while (i < pattern.size() && tmp != end) {
                char32_t cp = next_codepoint(tmp, end);
                if (cp != pattern[i++]) break;
            }
            if (i == pattern.size()) {
                return index;
            }
            next_codepoint(cur, end);
            it = cur;
            ++index;
        }
        return SIZE_MAX;
    }

    // ============================================================
    // Sostituisce un singolo codepoint
    // ============================================================
    void DUTF8String::replace(char32_t target, char32_t replacement) {
        std::string out;
        auto it = data.cbegin();
        auto end = data.cend();
        while (it != end) {
            auto start = it;
            char32_t cp = next_codepoint(it, end);
            if (cp == target)
                encode_utf8(replacement, out);
            else
                out.append(start, it);
        }
        data.swap(out);
    }

    // ============================================================
    // Sostituisce una sottostringa UTF-8
    // ============================================================
    void DUTF8String::replace(const std::string& search, const std::string& replace) {
        if (search.empty()) return;
        std::vector<char32_t> pattern;
        auto pit = search.cbegin();
        while (pit != search.cend())
            pattern.push_back(next_codepoint(pit, search.end()));

        std::string out;
        auto it = data.begin();
        auto end = data.end();
        while (it != end) {
            auto cur = it;
            auto tmp = it;
            size_t i = 0;
            while (i < pattern.size() && tmp != end) {
                char32_t cp = next_codepoint(tmp, end);
                if (cp != pattern[i++]) break;
            }
            if (i == pattern.size()) {
                out += replace;
                it = tmp;
            } else {
                char32_t cp = next_codepoint(it, end);
                encode_utf8(cp, out);
            }
        }
        data.swap(out);
    }

    void DUTF8String::insert(int codepoint, size_t pos) {
        auto it = data.cbegin();
        auto end = data.cend();
        size_t index = 0;
        while (it != end && index < pos) {
            next_codepoint(it, end);
            ++index;
        }
        std::string s;
        encode_utf8(codepoint, s);
        data.insert(std::distance(data.cbegin(), it), s);
    }
};