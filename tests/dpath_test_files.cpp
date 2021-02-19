#include <gtest/gtest.h>
#include <libdpp/DPath.h>
#include <libdpp/DFilesystem.h>


TEST(dpath_test, count_files_no_recoursive)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,false);
    ASSERT_EQ(4,n);
}

TEST(dpath_test, count_files_recoursive)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,true);
    ASSERT_EQ(8,n);
}

TEST(dpath_test, count_TXT_files_no_recoursive_no_case)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,false,"",false,"TXT",false,false);
    ASSERT_EQ(2,n);
}

TEST(dpath_test, count_TXT_files_no_recoursive_case)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,false,"",false,"TXT",false,true);
    ASSERT_EQ(1,n);
}

TEST(dpath_test, count_TXT_files_recoursive_no_cs)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,true,"",false,"TXT",false,false);
    ASSERT_EQ(4,n);
}

TEST(dpath_test, count_TXT_files_recoursive_cs)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,true,"",false,"TXT",false,true);
    ASSERT_EQ(2,n);
}
