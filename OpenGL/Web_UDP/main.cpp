#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Net.h"

using namespace std;
using namespace net;
int main()
{
	// initialize socket layer

	if (!InitializeSockets())
	{
		printf("failed to initialize sockets %d\n", WSAGetLastError());
		return 1;
	}

	// create socket

	int port = 30000;
	int port2 = 30001;

	printf("creating socket on port %d\n", port);

	Socket socket2;

	if (!socket2.Open(port2))
	{
		printf("failed to create socket!\n");
		return 1;
	}

	// send and receive packets to ourself until the user ctrl-breaks...

	while (true)
	{
		const char data[] = "hello world, to everyone!";

		socket2.Send(Address(127, 0, 0, 1, port), data, sizeof(data));
		printf("Sent packet to 127.0.0.1:30000");

		wait(0.25f);
	}

	// shutdown socket layer

	ShutdownSockets();

	return 0;
}