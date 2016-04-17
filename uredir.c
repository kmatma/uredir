/*
 * Copyright (C) 2007-2008  Ivan Tikhonov <kefeer@brokestream.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
	int sd;
	struct sockaddr_in a;
	struct sockaddr_in sa;
	struct sockaddr_in da;

	if (argc != 3 && argc != 5) {
		printf("Usage: %s our-ip our-port send-to-ip send-to-port\n", argv[0]);
		printf("Usage: %s our-ip our-port             # echo mode\n", argv[0]);
		exit(1);
	}

	sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sd < 0) {
		perror("Failed opening UDP socket");
		return 1;
	}
		

	a.sin_family = AF_INET;
	a.sin_addr.s_addr = inet_addr(argv[1]);
	a.sin_port = htons(atoi(argv[2]));
	if (bind(sd, (struct sockaddr *)&a, sizeof(a)) == -1) {
		printf("Can't bind our address (%s:%s)\n", argv[1], argv[2]);
		return 1;
	}

	if (argc == 5) {
		a.sin_addr.s_addr = inet_addr(argv[3]);
		a.sin_port = htons(atoi(argv[4]));
	}

	da.sin_addr.s_addr = 0;
	while (1) {
		char buf[BUFSIZ];
		int sn = sizeof(sa);
		int n;

		n = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&sa, &sn);
		if (n <= 0)
			continue;

		if (argc == 3) {
			sendto(sd, buf, n, 0, (struct sockaddr *)&sa, sn);
		} else if (sa.sin_addr.s_addr == a.sin_addr.s_addr && sa.sin_port == a.sin_port) {
			if (da.sin_addr.s_addr)
				sendto(sd, buf, n, 0, (struct sockaddr *)&da, sizeof(da));
		} else {
			sendto(sd, buf, n, 0, (struct sockaddr *)&a, sizeof(a));
			da = sa;
		}
	}

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
