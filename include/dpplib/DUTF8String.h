#ifndef DUTF8String_H
#define DUTF8String_H

#include <string>
#include <vector>
#include <memory>

namespace DTools
{

class DUTF8String {
    std::string data;

    DUTF8String(const std::vector<char32_t>& cps);

    /**
     * Trova il primo codepoint nel range it - end
     */
    template <typename It>
    static char32_t next_codepoint(It& it, It end) {
        if (it == end) throw std::out_of_range("End of string");

        unsigned char c = static_cast<unsigned char>(*it++);
        if (c < 0x80) return c;

        if ((c >> 5) == 0x6) {
            if (std::distance(it, end) < 1) throw std::runtime_error("Truncated UTF-8");
            char32_t cp = ((c & 0x1F) << 6) | (static_cast<unsigned char>(*it++) & 0x3F);
            return cp;
        }
        if ((c >> 4) == 0xE) {
            if (std::distance(it, end) < 2) throw std::runtime_error("Truncated UTF-8");
            char32_t cp = ((c & 0x0F) << 12)
                        | ((static_cast<unsigned char>(*it++) & 0x3F) << 6)
                        | (static_cast<unsigned char>(*it++) & 0x3F);
            return cp;
        }
        if ((c >> 3) == 0x1E) {
            if (std::distance(it, end) < 3) throw std::runtime_error("Truncated UTF-8");
            char32_t cp = ((c & 0x07) << 18)
                        | ((static_cast<unsigned char>(*it++) & 0x3F) << 12)
                        | ((static_cast<unsigned char>(*it++) & 0x3F) << 6)
                        | (static_cast<unsigned char>(*it++) & 0x3F);
            return cp;
        }

        throw std::runtime_error("Invalid UTF-8 sequence");
    }


    // ============================================================
    // Codifica un codepoint in UTF-8
    // ============================================================
    void encode_utf8(char32_t cp, std::string& out);

    // ============================================================
    // Conversioni base
    // ============================================================
    std::string to_string();

    std::string_view to_string_view(void) noexcept;

    // ============================================================
    // Conta i codepoints in una stringa UTF-8
    // ============================================================
    size_t length(void);

    // ============================================================
    // Estrae una sottostringa per codepoint
    // ============================================================
    std::string substr(size_t start_cp, size_t count_cp = SIZE_MAX);

    // ============================================================
    // Trova un singolo codepoint
    // ============================================================
    inline size_t find(char32_t target, size_t start_cp = 0);

    // ============================================================
    // Trova una sottostringa UTF-8
    // ============================================================
    size_t find(const std::string& sub, size_t start_cp = 0);

    // ============================================================
    // Sostituisce un singolo codepoint
    // ============================================================
    void replace(char32_t target, char32_t replacement);

    // ============================================================
    // Sostituisce una sottostringa UTF-8
    // ============================================================
    void replace(const std::string& search, const std::string& replace);

    void insert(int codepoint, size_t pos);
};
}

#endif