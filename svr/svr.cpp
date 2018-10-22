// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "dqsService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TDebugProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include<string>

#include<iostream>

#include"service_provider.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;


using boost::shared_ptr;
using std::cout;
using std::endl;

using ppconsul::ServiceProvider;

#define SVR_TAGS {"dqs", "base"}
#define SVR_NAME "jiot_dqs"


TSimpleServer* g_server = NULL;
ServiceProvider* g_svrProvider = NULL;



class dqsServiceHandler : virtual public dqsServiceIf
{
public:
    dqsServiceHandler()
    {
        // Your initialization goes here
        m_nSeqId = 0;
    }

    void QueryDeviceInfo(std::string& _return, const std::string& req)
    {
        // Your implementation goes here
        _return = req +" id : " + std::to_string(++m_nSeqId);
    }
private:
    int m_nSeqId;
};


void signalHandler(int nSignal ) //信号处理
{
	if ( SIGTERM == nSignal )
	{
		cout<<"receive SIGTERM signal."<<endl;
		if(g_svrProvider != NULL )
		{
			g_svrProvider->unregister();
		}
		
		if( g_server == NULL ) 
		{
			cout<<"g_server null detected"<<endl;
		}
		g_server->stop();
		cout<<"program exit now ..."<<endl;
		//exit(0);
	}
}




int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;


	if( signal(SIGTERM, signalHandler) == SIG_ERR )
	{
		cout << "init signal error " << endl;
		return -1;
	}

    int port = 9092;	

    ServiceProvider svrProvider({"127.0.0.1:8500"}, SVR_NAME, port);
    g_svrProvider = &svrProvider;
    if( !svrProvider.register1( SVR_TAGS ) ) {
        cout<< "svr register failed."<<endl;
    } else {
        cout<<"svr register success."<<endl;
    }


    /*  vector<Host> hosts;
        if( !ServiceProvider::parseHosts("192.168.33.10:2379", hosts) )
        {
            cout<<"parse etcd hosts error"<<endl;
        }
        ServiceProvider svrProvider(hosts, port);
        svrProvider.registerServiceOnce();
        svrProvider.registerServicePeriodly();
    */

    boost::shared_ptr<dqsServiceHandler> handler(new dqsServiceHandler());
    boost::shared_ptr<TProcessor> processor(new dqsServiceProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
//  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
//  boost::shared_ptr<TProtocolFactory> protocolFactory(new TDebugProtocolFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(new TJSONProtocolFactory());

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    g_server = &server;
    server.serve();
    return 0;
}

