// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "HelloSvc.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

class HelloSvcHandler : virtual public HelloSvcIf {
 public:
  HelloSvcHandler() {
    // Your initialization goes here
  }

  void hello_func(std::string& _return) {
    // Your implementation goes here
    printf("hello_func\n");
  }

  void hello_func2(const std::string& var1, const int64_t var2) {
    // Your implementation goes here
    printf("hello_func2\n");
  }

  void hello_func3(const std::string& var1, const Hello_struct& struct1, const MyInteger var3) {
    // Your implementation goes here
    printf("hello_func3\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  ::std::shared_ptr<HelloSvcHandler> handler(new HelloSvcHandler());
  ::std::shared_ptr<TProcessor> processor(new HelloSvcProcessor(handler));
  ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}
