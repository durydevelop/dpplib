#include <gtest/gtest.h>
#include "dpplib/DUTF8String.h"

using DTools::DUTF8String;

TEST(DUTF8StringTest, ConstructFromStdString) {
    DUTF8String s(std::string("hello"));
    EXPECT_EQ(s.to_string(), "hello");
}

TEST(DUTF8StringTest, ConstructFromStringView) {
    DUTF8String s(std::string_view("ciao"));
    EXPECT_EQ(s.to_string(), "ciao");
}

TEST(DUTF8StringTest, ConstructFromCString) {
    DUTF8String s("abc");
    EXPECT_EQ(s.to_string(), "abc");
}

TEST(DUTF8StringTest, MoveFromStdString) {
    std::string tmp = "move";
    DUTF8String s(std::move(tmp));
    EXPECT_EQ(s.to_string(), "move");
}

TEST(DUTF8StringTest, AssignFromStdString) {
    DUTF8String t;
    t = std::string("assigned");
    EXPECT_EQ(t.to_string(), "assigned");
}

TEST(DUTF8StringTest, AssignFromCString) {
    DUTF8String t;
    t = "from_cstr";
    EXPECT_EQ(t.to_string(), "from_cstr");
}

TEST(DUTF8StringTest, MoveAssignFromStdString) {
    std::string tmp = "moved_assign";
    DUTF8String t;
    t = std::move(tmp);
    EXPECT_EQ(t.to_string(), "moved_assign");
}

TEST(DUTF8StringTest, EmptyBehavior) {
    DUTF8String t;
    EXPECT_TRUE(t.empty());
    t = "not empty";
    EXPECT_FALSE(t.empty());
}

TEST(DUTF8StringTest, ConcatenationDUTF8String) {
    DUTF8String a("hello");
    DUTF8String b("_world");
    DUTF8String c = a + b;
    EXPECT_EQ(c.to_string(), "hello_world");
}

TEST(DUTF8StringTest, ConcatenationWithStdStringLeft) {
    std::string left = "left_";
    DUTF8String right("right");
    DUTF8String res = left + right;
    EXPECT_EQ(res.to_string(), "left_right");
}

TEST(DUTF8StringTest, ConcatenationWithCStringLeft) {
    const char* left = "c_";
    DUTF8String right("str");
    DUTF8String res = left + right;
    EXPECT_EQ(res.to_string(), "c_str");
}

TEST(DUTF8StringTest, CStrBehavior) {
    DUTF8String t("hello_c");
    EXPECT_STREQ(t.c_str(), "hello_c");
    // ensure pointer remains valid as long as object exists
    const char* p = t.c_str();
    EXPECT_EQ(std::string(p), t.to_string());
}

TEST(DUTF8StringTest, AssignStdString) {
    DUTF8String t;
    std::string s = "assigned_std";
    t.assign(s);
    EXPECT_EQ(t.to_string(), "assigned_std");
}

TEST(DUTF8StringTest, AssignCStringWithCount) {
    DUTF8String t;
    const char* s = "hello_world_extra";
    t.assign(s, 11); // "hello_world"
    EXPECT_EQ(t.to_string(), "hello_world");
}

TEST(DUTF8StringTest, AssignFromDUTF8String) {
    DUTF8String a("original");
    DUTF8String b;
    b.assign(a);
    EXPECT_EQ(b.to_string(), "original");
}

TEST(DUTF8StringTest, EraseAscii) {
    DUTF8String t("hello_world");
    t.erase(5, 1); // remove underscore
    EXPECT_EQ(t.to_string(), "helloworld");
}

TEST(DUTF8StringTest, EraseToEnd) {
    DUTF8String t("abcdef");
    t.erase(3); // erase from index 3 to end -> "abc"
    EXPECT_EQ(t.to_string(), "abc");
}

TEST(DUTF8StringTest, EraseWithUnicode) {
    // "a" + euro sign (3 bytes) + "b" -> total codepoints 3
    DUTF8String t("aF\u20ACb");
    // Note: source encoding in file may not preserve special codepoints portably,
    // so we'll construct using assign from parts to be safe.
    DUTF8String u;
    u.assign("a");
    // append euro sign U+20AC via codepoint insert
    u.insert_codepoint(0x20AC, 1); // insert at position 1
    u += DUTF8String("b");
    // now erase the euro sign (at pos 1)
    u.erase(1,1);
    EXPECT_EQ(u.to_string(), "ab");
}

TEST(DUTF8StringTest, IteratorAscii) {
    DUTF8String t("hello");
    std::string out;
    for (auto cp : t) out.push_back(static_cast<char>(cp));
    EXPECT_EQ(out, "hello");
}

TEST(DUTF8StringTest, IteratorCountUnicode) {
    DUTF8String t;
    t.assign("a");
    t.insert_codepoint(0x20AC, 1); // euro sign
    t += DUTF8String("b");
    size_t count = 0;
    for (auto cp : t) ++count;
    EXPECT_EQ(count, 3u);
}
