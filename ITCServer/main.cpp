#include "ItcServer.h"
#include <thread>
#include <string>

using namespace std;
/*
void consumer_process(ItcClient<string, double>& itcClient)
{

}

void producer_process(ItcServer<string, double>& itcServer)
{

}
*/

int main()
{
    ItcServer<string, double> itcServer;
    auto client = itcServer.get_client();
    int abc = 1;
}