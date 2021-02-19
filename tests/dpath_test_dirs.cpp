#include <gtest/gtest.h>
#include <libdpp/DPath.h>
#include <libdpp/DFilesystem.h>

TEST(dpath_test, count_dirs_no_recoursive)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountDirs(SourcePath,false);
    ASSERT_EQ(2,n);
}


TEST(dpath_test, count_dirs_recoursive)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountDirs(SourcePath,true);
    ASSERT_EQ(4,n);
}

TEST(dpath_test, count_COM_dirs_no_recoursive_no_case)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountDirs(SourcePath,false,"",false,"COM",false,false);
    ASSERT_EQ(2,n);
}

TEST(dpath_test, count_COM_dirs_no_recoursive_case)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountDirs(SourcePath,false,"",false,"COM",false,true);
    ASSERT_EQ(1,n);
}

TEST(dpath_test, count_COM_dirs_recoursive_no_cs)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountDirs(SourcePath,true,"",false,"COM",false,false);
    ASSERT_EQ(4,n);
}

TEST(dpath_test, count_COM_dirs_recoursive_cs)
{
    DTools::fs::path SourcePath="./find";
    size_t n=DTools::DPath::CountDirs(SourcePath,true,"",false,"COM",false,true);
    ASSERT_EQ(2,n);
}
