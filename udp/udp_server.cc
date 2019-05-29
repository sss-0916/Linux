#include "udp_socket.h"

int main(int argc, char* argv[]){
	bool ret;

	if(argc != 3){
		cout << "./udp_server ip port" << endl;
		return -1;
	}

	std::string ip = argv[1];
	uint16_t port = atoi(argv[2]);

	UdpSocket sock;

	ret = sock.Socket();
	if(!ret){
		return -1;
	}

	ret = sock.Bind(ip, port);
	if(!ret){
		return -1;
	}

	while(1){
		std::string buf;
		struct sockaddr_in cli_addr;

		ret = sock.Recv(buf, &cli_addr);
		if(!ret){
			return -1;
		}

		cout << "client say: " << buf << endl;
		cout << "server say: " << endl;
		fflush(stdout);
		std::cin >> buf;

		ret = sock.Send(buf, &cli_addr);
		if(!ret){
			return -1;
		}
	}

	ret = sock.Close();
	if(!ret){
		return -1;
	}

	return 0;
}
