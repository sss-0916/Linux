#include "udp_socket.h"

int main(int argc, char* argv[]){
	bool ret;

	if(argc != 3){
		cout << "./udp_client ip port" << endl;
	}

	std::string ip = argv[1];
	uint16_t port = atoi(argv[2]);

	UdpSocket sock;
	ret = sock.Socket();

	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &ser_addr.sin_addr);

	while(1){
		std::string buf;

		cout << "client say: ";
		fflush(stdout);

		std::cin >> buf;

		ret = sock.Send(buf, &ser_addr);
		if(!ret){
			return -1;
		}

		ret = sock.Recv(buf, &ser_addr);
		if(!ret){
			return -1;
		}

		cout << "server say: " << buf << endl;
	}
	
	ret = sock.Close();
	if(!ret){
		return -1;
	}

	return 0;
}
