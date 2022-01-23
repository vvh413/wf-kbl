#include <chrono>
#include <iostream>
#include <ratio>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <thread>


using namespace std;
using namespace chrono;


#define SERVER_PATH "/tmp/kbl_server"
#define CLIENT_PATH "/tmp/kbl_client"


class Client {

public:

	Client () {}

	Client(const char* server_path, const char* client_path) {
		if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
	        close(fd);
	        throw runtime_error("socket error");
	    }

	    set_addr(&server_addr, server_path);
	    set_addr(&client_addr, client_path);
	    
	    unlink(client_path);
	    if (bind(fd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
	        close(fd);
	        throw runtime_error("bind error");
	    }
	}

	void start () {
		char layout[2];

		while (true) {
			request();
			recv_data(layout, sizeof(layout));      
		 	cout << layout << endl;
		 	this_thread::sleep_for(milliseconds(100));
		}

		close(fd);
	}

private:

	int fd;
	struct sockaddr_un server_addr, client_addr;

	void set_addr (struct sockaddr_un* addr, const char* path) {
		memset(addr, 0, sizeof(*addr));
	    addr->sun_family = AF_UNIX;
	    strcpy(addr->sun_path, path);
	}

	void request () {
		if (sendto(fd, NULL, 0, 0, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
	 		close(fd);
	 		throw runtime_error("request error");
	    }
	}

	void recv_data (void* data, size_t size) {
		if (recvfrom(fd, data, size, 0, NULL, NULL) < 0) {
	 		close(fd);
	 		throw runtime_error("recv error");
	 	}
	}

};


int main () {

	const auto now = high_resolution_clock::now();
	const long int timestamp = duration_cast<microseconds>(now.time_since_epoch()).count();
	const string client_path = string(CLIENT_PATH) + "_" + to_string(timestamp);

	Client client (SERVER_PATH, client_path.c_str());
	client.start();
	
}
