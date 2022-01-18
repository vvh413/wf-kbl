#include <chrono>
#include <iostream>
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
		int layout_index;

		while (true) {
			request();
			layout_index = recv_data();      
		 	cout << get_layout_by_index(layout_index) << endl;
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

	int recv_data () {
		int layout_index = -1;
		if (recvfrom(fd, &layout_index, sizeof(layout_index), 0, NULL, NULL) < 0) {
	 		close(fd);
	 		throw runtime_error("recv error");
	 	}
	 	return layout_index;
	}

	static const char* get_layout_by_index (int index) {
		switch (index) {
			case 0:
				return "us";
			case 1:
				return "ru";
			default:
				return "--";
		}
	}

};


int main () {

	const auto now = high_resolution_clock::now();
	const int timestamp = duration_cast<milliseconds>(now.time_since_epoch()).count();
	const string client_path = string(CLIENT_PATH) + "_" + to_string(timestamp);

	Client client (SERVER_PATH, client_path.c_str());
	client.start();
	
}
