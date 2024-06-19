#include <gtest/gtest.h>
#include <dpplib/DPath.h>
#include <dpplib/DFilesystem.h>

using namespace DTools;

bool StopRequest=false;

TEST(DPathTest, CountDirsNoRecoursive)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountDirs(SourcePath,false,StopRequest);
    ASSERT_EQ(1,n);
}


TEST(dpath_test, count_dirs_recoursive)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountDirs(SourcePath,true,StopRequest);
    ASSERT_EQ(2,n);
}

TEST(dpath_test, count_COM_dirs_no_recoursive_no_case)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountDirs(SourcePath,false,"",false,"COM",false,false,StopRequest);
    ASSERT_EQ(1,n);
}

TEST(dpath_test, count_COM_dirs_no_recoursive_case)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountDirs(SourcePath,false,"",false,"COM",false,true,StopRequest);
    ASSERT_EQ(1,n);
}

TEST(dpath_test, count_COM_dirs_recoursive_no_case)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountDirs(SourcePath,true,"",false,"COM",false,false,StopRequest);
    ASSERT_EQ(2,n);
}

TEST(dpath_test, count_COM_dirs_recoursive_case)
{
    fs::path SourcePath="./find";
    size_t n=DPath::CountDirs(SourcePath,true,"",false,"COM",false,true,StopRequest);
    ASSERT_EQ(2,n);
}

TEST(dpath_test, move_OLD_dirs_recoursive_case)
{
    // Source
    fs::path SourcePath="./move/from/";
    int n=DPath::CountDirs(SourcePath,false,StopRequest);
    ASSERT_EQ(1,n);
    fs::path SourceDir=SourcePath / "subdir1.OLD";
    int nSourceFiles=DPath::CountFiles(SourceDir,true,StopRequest);
    // Dest
    fs::path DestPath="./move/to/";
    n=DPath::CountDirs(DestPath,false,StopRequest);
    ASSERT_EQ(0,n);
    fs::path DestDir=DestPath / "subdir1.OLD";
    // Move
    DPath::MoveDir(SourceDir,DestDir,false);
    // Check dest
    n=DPath::CountDirs(DestPath,false,StopRequest);
    ASSERT_EQ(1,n);
    int nDestFiles=DPath::CountFiles(DestDir,true,StopRequest);
    ASSERT_EQ(nDestFiles,nSourceFiles);
    // Get back
    DPath::MoveDir(DestDir,SourceDir,false);
    n=DPath::CountDirs(DestPath,false,StopRequest);
    ASSERT_EQ(0,n);
    n=DPath::CountDirs(SourcePath,false,StopRequest);
    ASSERT_EQ(1,n);
}
