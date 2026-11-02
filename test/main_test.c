#include "unity_fixture.h"

TEST_GROUP_RUNNER(Reader);
TEST_GROUP_RUNNER(Parser);

static void run_all()
{
	RUN_TEST_GROUP(Reader);
	RUN_TEST_GROUP(Parser);
}

int main(int argc, const char *argv[])
{
	return UnityMain(argc, argv, run_all);
}
