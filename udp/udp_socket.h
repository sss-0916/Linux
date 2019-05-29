#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using std::cout;
using std::endl;

class UdpSocket{
	public:
		UdpSocket() : _sockfd(-1){
		}

		~UdpSocket(){
		}

		bool Socket(){
			_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if(_sockfd < 0){
				perror("socket error");
				return false;
			}

			return true;
		}

		bool Bind(std::string& ip, uint16_t port){
			struct sockaddr_in addr;

			addr.sin_family = AF_INET;
			inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
			addr.sin_port = htons(port);

			socklen_t len = sizeof(struct sockaddr_in);

			int ret = bind(_sockfd, (struct sockaddr*)&addr, len);
			if(ret < 0){
				perror("bind error");
				return false;
			}

			return true;
		}

		bool Recv(std::string buf, struct sockaddr_in* saddr){
			char temp[1024] = {0};

			socklen_t len = sizeof(struct sockaddr_in);

			int ret = recvfrom(_sockfd, temp, 1024, 0, 
					(struct sockaddr*)saddr, &len);
			if(ret < 0){
				perror("recvfrom error");
				return false;
			}

			buf.assign(temp, ret);

			return true;
		}

		bool Send(std::string buf, struct sockaddr_in* daddr){
			socklen_t len = sizeof(struct sockaddr_in);

			int ret = sendto(_sockfd, buf.c_str(), buf.size(), 0, 
					(struct sockaddr*)daddr, len);
			if(ret < 0){
				return false;
			}

			return true;
		}

		bool Close(){
			int ret = close(_sockfd);
			if(ret < 0){
				return false;
			}

			return true;
		}

	private:
		int _sockfd;
};
