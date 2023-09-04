#include <gtest/gtest.h>
#include <libdpp/DPath.h>
#include <libdpp/DFilesystem.h>

bool StopRequest=false;

TEST(dpath_test, count_files_no_recoursive)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,false,StopRequest);
    ASSERT_EQ(4,n);
}

TEST(dpath_test, count_files_recoursive)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,true,StopRequest);
    ASSERT_EQ(12,n);
}

TEST(dpath_test, count_TXT_files_no_recoursive_no_case)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,false,"",false,"TXT",false,false,StopRequest);
    ASSERT_EQ(2,n);
}

TEST(dpath_test, count_TXT_files_no_recoursive_case)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,false,"",false,"TXT",false,true,StopRequest);
    ASSERT_EQ(1,n);
}

TEST(dpath_test, count_TXT_files_recoursive_no_cs)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,true,"",false,"TXT",false,false,StopRequest);
    ASSERT_EQ(6,n);
}

TEST(dpath_test, count_TXT_files_recoursive_cs)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountFiles(SourcePath,true,"",false,"TXT",false,true,StopRequest);
    ASSERT_EQ(3,n);
}
