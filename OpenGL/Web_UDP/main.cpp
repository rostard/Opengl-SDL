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

	Socket socket;

	if (!socket.Open(port))
	{
		printf("failed to create socket!\n");
		return 1;
	}

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
		const char data[] = "hello world!";

		socket2.Send(Address(127, 0, 0, 1, port), data, sizeof(data));

		while (true)
		{
			Address sender;
			unsigned char buffer[256];
			int bytes_read = socket.Receive(sender, buffer, sizeof(buffer));
			if (!bytes_read)
				break;
			printf("received packet from %d.%d.%d.%d:%d (%d bytes)\n",
				sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(),
				sender.GetPort(), bytes_read);
		}

		wait(0.25f);
	}

	// shutdown socket layer

	ShutdownSockets();

	return 0;
}