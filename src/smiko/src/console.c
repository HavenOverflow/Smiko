/* Copyright 2025 HavenOverflow
 * Use of this code is permissible so long as the appropriate credit
 * is provided. See the LICENSE file for more info.
 */

#include <fcntl.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

#include "console.h"

int serialport_init(const char* serialport, int baud)
{
	int fd;
	
	fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) return -1;
	
	struct termios toptions;

	tcgetattr(fd, &toptions);
	speed_t brate = B115200; // This is the H1 UART BAUD rate
	cfsetispeed(&toptions, brate);
	cfsetospeed(&toptions, brate);

	// 8N1
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	toptions.c_cflag &= ~CRTSCTS; // No flow control
	toptions.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	// Make raw
	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	toptions.c_oflag |= (OPOST | ONLCR);

	toptions.c_cc[VMIN] = 0;
	toptions.c_cc[VTIME] = 20;

	tcsetattr(fd, TCSANOW, &toptions);
	
	return fd;
}

int serialport_disconected(int fd, bool a){
	if(a){ //user knows console is disconnected
		return 0;
	}

	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN | POLLERR | POLLHUP;
	int ret = poll(&pfd, 1, 1);

	if(ret < 0)
		return -1;
	
	if(pfd.revents & (POLLHUP | POLLERR))
		return -1;

	return 0;
}

int serialport_read(int fd, char *c)
{
	int n, i = 0;

	do {
		i++;
		n = read(fd, c, 1); // Read one char at a time
		fflush(NULL);

		if(n == -1) return -1;
		if(n == 0 && i > 10000) return -1;
		if(n == 0) continue;
	} while(n <= 0);
	
	return 0;
}

void launch_console(char *path)
{
	int input = serialport_init("/dev/stdin", B115200);
    int con = serialport_init(path, B115200);
    char c = ' ';
	static int last_was_nl = 0;
	bool con_gone = false;

	if(con == -1){
		fprintf(stderr, "Error: Failed to open serial port %s\nPlease connect device or press Ctrl+C to quit.\n", path);
		con_gone = true;
	}

    while (true) {
		if(serialport_disconected(con, con_gone)){
			printf("\n[Smiko: Serial device disconnected. Reconnect device or Ctrl+C to quit]\n");
			con_gone = true;
			close(con); //so that we can re-connect
		}

		if(con_gone){
			con = serialport_init(path, B115200);
			if(con < 0) sleep(1);
			else con_gone = false;
		}

		if (serialport_read(input, &c) != -1) {
			if (c == 0x3) // CTRL + C 
				return;

			write(con, &c, sizeof(char));
			fflush(stdout);
		}
        if (serialport_read(con, &c) == -1) 
            continue;

		// probably a better way to do this...
		if (c == '\r') continue;

		if (c == '\n') {
			if (last_was_nl)
				continue;
			printf("\r\n");
			last_was_nl = 1;
		} else {
			printf("%c", c);
			last_was_nl = 0;
		}
		
	}
}
