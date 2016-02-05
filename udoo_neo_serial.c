/**
*  Copyright (C) 2014 Ekironji <ekironjisolutions@gmail.com>
*
*  This file is part of serial libraries examples for UDOO
*
*  Serial libraries examples for UDOO is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This libraries are distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h> //for fd_set
#include <sys/time.h>  //for timeval
#include <time.h>


#define BUFFER_MAX 64


//Utility to configure the serial parameters
int set_interface_attribs (int fd, int speed, int parity){
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

//Utility to set blocking on or off
void set_blocking (int fd, int should_block){
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            	// 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf ("error %d setting term attributes", errno);
}

//Open the serial port file descriptor
int openSerial(){
	char *portname = "/dev/ttyMCC";
	
	// Open serial port (file descriptor)
	int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
			printf("error %d opening %s: %s", errno, portname, strerror (errno));
			return;
	}
	
	// Configure the serial communication interface before use
	set_interface_attribs (fd, B115200, 0);		// set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (fd, 0);						// set no blocking
	return fd;
}

//Utility to get size of unsigned char* (char array)
int stringSize(unsigned char* data){
	int dataSize = 0;
	while(data[dataSize]!=0){	// end of string == 0
		dataSize++;
	}
	return dataSize;	
}

//Parses the command out of the input buffer into the cmd variable
//Copies the characters between : and ( if found.
void getCmd(unsigned char* buff, unsigned char* cmd){
	int i;
	int found = 0;
	
	while(buff[i] != 0)
	{
		if(buff[i] == ':') found = i;
		if(found){
			if(buff[i] == '(') break;
			cmd[i-found-1] = buff[i];
		}
		i++;
	}
	
	return;
}

//Parses the parameters out of the input buffer into the par variable
//Copies the characters between : and ( if found.
void getPar(unsigned char* buff, unsigned char* par){
	int i;
	int found = 0;
	
	while(buff[i] != 0)
	{
		if(buff[i] == '(') found = i;
		if(found){
			if(buff[i] == ')') break;
			par[i-found-1] = buff[i];
		}
		i++;
	}
	
	return;
}

//Parses the command out of the input buffer into the cmd variable
void getCommand(unsigned char* cmd, unsigned char* buff, int size){
	int i;
	int found = 0;
	
	while(buff[i] != 0)
	{
		if(buff[i] == ':') found = i;
		if(found){
			if(buff[i] == '(') break;
			cmd[i-found-1] = buff[i];
		}
		i++;
	}
	
	/*
	 * Alternative way
	for(i = 0; i < size; i++)
	{
		if(buff[i] == '(') break;
		if(found){ 
			cmd[i-found-1] = buff[i];
		}
		if(buff[i] == ':') found = i;
	}
	*/
	
	return;
}

//Print to stdout the characters in a char array
void printText(char* text){
	int i = 0;
	while(text[i] != 0)
	{
		printf("%d ",text[i++]);
	}
}

//Compares two strings to see if they are equal
//Returns 1 if equal, 0 if they are not equal
int compareText(unsigned char* text1, unsigned char* text2)
{
	int size1 = stringSize(text1);
	int size2 = stringSize(text2);
		
	if(size1 == size2){
		int i;
		for(i=0;i<size1;i++){
			if(text1[i] != text2[i]){
				//Different characters
				return 0;
			}
		}
		//Identical strings
		return 1;
	}
	//Different sizes
	return 0;
}



//Main program
int main(void) {
	int dataAvailable,receivedBytes, sentBytes;
	unsigned char outBuff[BUFFER_MAX];
	unsigned char inBuff[BUFFER_MAX];
	unsigned char* cmd = malloc(BUFFER_MAX * sizeof(char));
	unsigned char* par = malloc(BUFFER_MAX * sizeof(char));
	
	// Open serial file descriptor
	int fd = openSerial();
		
	//Loop to scan
	while(1){
		receivedBytes = read(fd,inBuff,BUFFER_MAX);
		
		if(receivedBytes > 0){						// Data found!

			int i,result;
			getCmd(inBuff,cmd);
			getPar(inBuff,par);
			
			if(compareText(cmd,"Debug")){
				//printf("Debug command received\n");
				time_t now = time(NULL);
				printf("%s ",par);
				printf("%s\n",ctime(&now));	
			}
			
		}else if(receivedBytes == 0){				//No data yet! go back to loop
			continue;					
		}else if(receivedBytes < 0){				//Error reading, exit.
			printf("Error reading from file!\n");
			perror("Error: " );
			close(fd);
			return -1;
		}
		usleep(50000);	// poll time approx 50mS (faster crashes the app)
	}
	
	free((void*)cmd);
	free((void*)par);	
	
	//Close serial's file descriptor
	close(fd);	
}
