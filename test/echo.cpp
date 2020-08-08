#include <iostream>
#include "muduo/net/TcpServer.h"

#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"

#include <functional>
#include <utility>
#include <iostream>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

class TransferServer{
	public:
	TransferServer(EventLoop* loop,InetAddress& listenAddr)
		:loop_(loop),
		 server_(loop_,listenAddr,"TransferServer")
	{
		server_.setConnectionCallback(
				std::bind(&TransferServer::onConnection,this,_1));
		server_.setMessageCallback(
				std::bind(&TransferServer::onMessage,this,_1,_2,_3));
	}
	
	~TransferServer(){}
	void start(){
		server_.start();
	}
	private:
	void onConnection(const TcpConnectionPtr& conn){
	//	std::cout<<conn->peerAddress().toIpPort().data()
	//			<<"->"<<conn->localAddress().toIpPort().data()
	//			<<(conn->connected()?"UP":"DOWN");
	}

	void onMessage(const TcpConnectionPtr& conn,
								Buffer* buf,Timestamp time){
		string msg(buf->retrieveAllAsString());
		std::cout<<conn->name()<<" recv "<<msg.size()<<" bytes at "<<time.toString();
		
		//reverse(msg.begin(),msg.end());
		conn->send(msg);
	}
	
	EventLoop* loop_;
	TcpServer server_;
};
int main(){
	EventLoop loop;
	InetAddress serverAddr(9000);

	TransferServer server(&loop,serverAddr);

	server.start();
	loop.loop();

	return 0;
}
