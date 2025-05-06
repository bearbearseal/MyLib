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
    printf("Server has message: %s\n", itcServer.has_message() ? "Yes" : "No");
    auto toServerMessage = itcServer.get_message();
    printf("Server has message: %s\n", toServerMessage.has_value() ? "Yes" : "No");
    printf("Client has message: %s\n", client.has_message() ? "Yes" : "No");
    auto messageFromServer = client.get_message();
    printf("Client has message: %s\n", messageFromServer.has_value() ? "Yes" : "No");
}