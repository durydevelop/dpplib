#include <gtest/gtest.h>
#include <dpplib/DPath.h>
#include <dpplib/DFilesystem.h>

using namespace DTools;

bool StopRequest=false;

TEST(dpath_test, count_files_no_recoursive)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountFiles(SourcePath,false,StopRequest);
    ASSERT_EQ(4,n);
}

TEST(dpath_test, count_files_recoursive)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountFiles(SourcePath,true,StopRequest);
    ASSERT_EQ(12,n);
}

TEST(dpath_test, count_TXT_files_no_recoursive_no_case)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountFiles(SourcePath,false,"",false,"TXT",false,false,StopRequest);
    ASSERT_EQ(2,n);
}

TEST(dpath_test, count_TXT_files_no_recoursive_case)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountFiles(SourcePath,false,"",false,"TXT",false,true,StopRequest);
    ASSERT_EQ(1,n);
}

TEST(dpath_test, count_TXT_files_recoursive_no_cs)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountFiles(SourcePath,true,"",false,"TXT",false,false,StopRequest);
    ASSERT_EQ(6,n);
}

TEST(dpath_test, count_TXT_files_recoursive_cs)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountFiles(SourcePath,true,"",false,"TXT",false,true,StopRequest);
    ASSERT_EQ(3,n);
}
