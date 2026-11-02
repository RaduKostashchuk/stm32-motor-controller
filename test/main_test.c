#include "unity_fixture.h"

TEST_GROUP_RUNNER(Reader);
TEST_GROUP_RUNNER(Parser);
TEST_GROUP_RUNNER(Formatter);
TEST_GROUP_RUNNER(Estimator);
TEST_GROUP_RUNNER(Controller);

static void run_all()
{
	RUN_TEST_GROUP(Reader);
	RUN_TEST_GROUP(Parser);
	RUN_TEST_GROUP(Formatter);
	RUN_TEST_GROUP(Estimator);
	RUN_TEST_GROUP(Controller);
}

int main(int argc, const char *argv[])
{
	return UnityMain(argc, argv, run_all);
}
