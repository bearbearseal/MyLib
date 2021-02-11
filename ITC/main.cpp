#include "TestITC.hpp"
#include "TestReadWrite.hpp"
//#include "TestWaitMessage.hpp"
//#include "TestDestroyItc.hpp"

int main()
{
	printf("Grand begin.\n");
	TestITC::test_run();
	TestReadWrite::test_2_vs_1();
	//TestWaitMessage::run();
	//TestDestroyItc::run();
	printf("Grand end.\n");
}
