#include <gtest/gtest.h>
#include <libdpp/DUpdate.h>

TEST (dupdate_test, autodetect_repo)
{
	EXPECT_THROW(DUpdate dUpdate());
}