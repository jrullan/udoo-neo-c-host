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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h> //for fd_set
#include <sys/time.h>  //for timeval
#include <time.h>
#include <pthread.h>


#define BUFFER_MAX 128	// Originally I had this set to 64
						// theoretically max size of arduino serial buffer
						// but apparently the Neo implementation supports
						// a higher limit.
						
#define PARS 10			// Max parameters to hold
#define PARS_SIZE 24	// Max size of characters in each parameter

const char* VALID_COMMANDS[] = {
	"Database","Debug","Email","HTTPRequest","Log","Webcam"
};

//Array for parameters comming from arduino
char PARAMETERS[PARS][PARS_SIZE];


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
			return 0;
	}
	
	// Configure the serial communication interface before use
	set_interface_attribs (fd, B115200, 0);		// set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (fd, 0);						// set no blocking
	return fd;
}

//Utility to get size of unsigned char* (char array)
int stringSize(char* data){
	int dataSize = 0;
	while(data[dataSize]!=0){	// end of string == 0
		dataSize++;
	}
	return dataSize;	
}

//Print to stdout the characters in a char array
void printText(unsigned char* text){
	int i = 0;
	while(text[i] != 0)
	{
		printf("%c",text[i]);
		i++;
	}
	//printf("\n");
}

//Compares two strings to see if they are equal
//Returns 1 if equal, 0 if they are not equal
int compareText(char* text1, char* text2){
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

// Function to append a string to another.
void appendString(char* text, char* app){
	int size1 = stringSize(text);
	int size2 = stringSize(app);
	int i;
	for(i=size1;i<size1+size2;i++){
		text[i] = app[i-size1];
	}
	text[size1+size2] = '\0';
	return;
}

// Function to clear a string.
void clearString(char* text){
	int size = stringSize(text);
	//printf("Clearing %d characters\n",size);
	memset(text,0,size);
}

//Parses the command out of the input buffer into the cmd variable
//Copies the characters between : and ( if found.
void getCmd(unsigned char* buff,  char* cmd){
	int i=0;	// WHY DID I FORGET TO INITIALIZE IT TO 0??????
	int found = 0;
	clearString(cmd);
	
	while(buff[i] != 0)
	{
		if(buff[i] == ':') found = ++i;
		if(found){
			if(buff[i] == '(') break;
			cmd[i-found] = buff[i];	
		}
		i++;
	}
	return;
}

//Parses the parameters out of the input buffer into the par variable
//Copies the characters between : and ( if found.
void getPar(unsigned char* buff,  char* par){
	int i = 0; // WHY DID I FORGET TO INITIALIZE IT TO 0??????
	int found = 0;			
	clearString(par);
	
	while(buff[i] != 0)
	{	
		if(buff[i] == '(') found = ++i;
		if(found){
			if(buff[i] == ')') break;
			par[i-found] = buff[i];
		}
		i++;
	}
	return;
}

//Checks if cmd correspond to any of the valid commands defined 
//in VALID_COMMANDS
int validCommand(char* cmd){
	int i;
	int size = sizeof VALID_COMMANDS/ sizeof (char*);
	for(i=0; i<size;i++){
		//printf("Comparing %s to %s...\n",cmd,VALID_COMMANDS[i]);
		if(compareText(cmd,(char*) VALID_COMMANDS[i])) return 1;
	}	
	return 0;
}

//Parse the parameters into the PARAMETERS array
//Parameters must be divided by commas
int parseParameters(char* par){
	int i = 0;
	int number = 0;
	int pos = 0;
	int p;

	//Clear all parameters
	for(p=0;p<PARS;p++){
		clearString(PARAMETERS[p]);
	}
	
	//Extract parameters into the PARAMETERS array
	while(par[i]!=0){
		if(par[i] == ','){
			number++;
			pos = i+1;	// add one to ignore the ',' 
		}else{
			PARAMETERS[number][i-pos]=par[i];
		}
		i++;
	}
	
	return number+1; // Number of parameters found
}


//---------------------------------------------------------------------
// MULTI THREADING SECTION
//---------------------------------------------------------------------

//Structure to pass command and parameters info to functions in threads
typedef struct commandStructure{
	char* cmd;
	char* par[PARS];
} commandStructure;

//Debug function to be called in a "detached" thread
void *debugFunc(void *arg){
	printf("============================\nIn debug thread:\n");
	struct commandStructure *command = (commandStructure *) arg;
	char cmd[PARS_SIZE];
	char message[PARS_SIZE];
	time_t now = time(NULL);
	
	//Copy command info locally
	strcpy(cmd,(const char*) command->cmd);
	strcpy(message,(const char*) command->par[0]);
	
	printf("cmd: %s\n",cmd);
	printf("par: %s\n",message);
	printf("time: %s\n",ctime(&now));
	
	pthread_exit(NULL);
}

//Log function to be called in a "detached" thread
void *logFunc(void *arg){
	printf("============================\nIn log thread:\n");
	struct commandStructure *command = (commandStructure *) arg;
	char cmd[PARS_SIZE];
	char message[PARS_SIZE];
	time_t now = time(NULL);	
	
	//Copy command info locally
	strcpy(cmd,(const char*) command->cmd);
	strcpy(message,(const char*) command->par[0]);

	FILE* fp;
	fp = fopen("/home/udooer/udoo_host_log.txt","a");
	fprintf(fp,"%s %s",message,ctime(&now));
	fclose(fp);	

	pthread_exit(NULL);
}

//Email function to be called in a "detached" thread
void *emailFunc(void *arg){
	printf("============================\nIn email thread:\n");
	struct commandStructure *command = (commandStructure *) arg;
	char cmd[PARS_SIZE];
	char emailAddress[PARS_SIZE];
	char emailSubject[PARS_SIZE];
	char emailMessage[PARS_SIZE];
	time_t now = time(NULL);
	
	//Copy command info locally
	strcpy(cmd,(const char*) command->cmd);
	strcpy(emailAddress,(const char*) command->par[0]);
	strcpy(emailSubject,(const char*) command->par[1]);
	strcpy(emailMessage,(const char*) command->par[2]);

	int status;
	char* email[stringSize(emailAddress)+5];
	char* subject[stringSize(emailSubject)+10];
	
	clearString((char*)email);
	appendString((char*)email,"To: ");
	appendString((char*)email,emailAddress);
	appendString((char*)email,"\n");				
	
	clearString((char*)subject);
	appendString((char*)subject,"Subject: ");
	appendString((char*)subject,emailSubject);
	appendString((char*)subject,"\n");				

	//1. Open email file
	FILE* fp;
	fp = fopen("/home/udooer/mail.txt","w+");
	
	//2. Write To, From, Subject and Contents
	fputs((char*)email,fp);
	fputs("From: udooneo@udooneo.com\n",fp);
	fputs((char*)subject,fp);
	fputs("\n",fp);
	fprintf(fp,"%s\n",emailMessage);
	fprintf(fp,"%s\n",ctime(&now));
	fputs("\n",fp);
	fputs("Message sent by Udoo Neo.\n",fp);
	fputs("=========================\n",fp);
	fputs("\n",fp);

	//3. Close file
	fclose(fp);
	status = system("cat ~/mail.txt");
	if(status == -1) printf("Error: could not execute command\n");
	
	//4. send email
	char ssmtpCommand[40];
	strcpy(ssmtpCommand, "ssmtp ");
	appendString((char*) ssmtpCommand, emailAddress);
	strcat(ssmtpCommand, " < ~/mail.txt");
	printf("Command to execute: %s\n",ssmtpCommand);
	status = system(ssmtpCommand);
	if(status == -1){ 
		printf("Error: could not send email\n");
	}else{
		printf("Email sent successfully! on %s\n",ctime(&now));
	}
	
	pthread_exit(NULL);
}

//Webcam function to be called in a "detached" thread
void *webcamFunc(void *arg){
	printf("============================\nIn debug thread:\n");
	struct commandStructure *command = (commandStructure *) arg;
	char cmd[PARS_SIZE];
	char message[PARS_SIZE];
	time_t now = time(NULL);
	int status;
	
	//Copy command info locally
	strcpy(cmd,(const char*) command->cmd);
	strcpy(message,(const char*) command->par[0]);
	
	status = system("streamer -c /dev/video1 -s 640x480 -o ~/capture.jpeg");
	if(status == -1){
		printf("Error: Could not take picture from webcam\n");
	}else{
		printf("Picture taken successfully!\n");
	}
	printf("cmd: %s\n",cmd);
	printf("par: %s\n",message);
	printf("time: %s\n",ctime(&now));
	
	pthread_exit(NULL);
}
//---------------------------------------------------------------------
// MAIN PROGRAM
//---------------------------------------------------------------------

//Main program
int main(void) {
	int receivedBytes;//, sentBytes;
	//unsigned char outBuff[BUFFER_MAX];
	unsigned char inBuff[BUFFER_MAX];
	//Reserve memory
	char* cmd = malloc((BUFFER_MAX+1) * sizeof(char));
	char* par = malloc((BUFFER_MAX+1) * sizeof(char));
	struct commandStructure command;
	
	//Threads variables
	pthread_t logThread;
	pthread_t emailThread;
	pthread_t debugThread;
	pthread_t webcamThread;
	
	// Open serial file descriptor
	int fd = openSerial();
		
	//Loop to scan
	while(1){
		receivedBytes = read(fd,inBuff,BUFFER_MAX);
		if(receivedBytes > 0){						// Data found!
			printf("\nPayload size: %d\n",receivedBytes);

			getCmd(inBuff,cmd);
			getPar(inBuff,par);
			int pars = parseParameters(par);
			
			if(!validCommand(cmd)){
				printf("Invalid Command: %s\n\n",cmd);
				continue;
			}else{	
				//printf("Command: %s\n",cmd);
				//int i = 0;
				//printf("Parameters found: %d\n",pars);
				//for(i=0;i<pars;i++) printf("Parameter %d - %s\n",i,PARAMETERS[i]);			
			}
			
			if(compareText(cmd,"Debug")){ //thread is detached so resources can be recycled.
				command.cmd = cmd;
				command.par[0] = PARAMETERS[0];

				//=======Call debugFunc in thread======
				int rc;
				pthread_attr_t attr;
				pthread_attr_init(&attr);
				pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
				
				if((rc = pthread_create(&debugThread,&attr,debugFunc,&command))){
					fprintf(stderr,"Error: Could not create thread: %d\n",rc);
				}
				
				pthread_attr_destroy(&attr);
			}

			if(compareText(cmd,"Log")){
				if(pars < 1){
					printf("Error: No message sent\n");
					continue;
				}
				command.cmd = cmd;
				command.par[0] = PARAMETERS[0];

				//=======Call logFunc in thread======
				int rc;
				pthread_attr_t attr;
				pthread_attr_init(&attr);
				pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
				
				if((rc = pthread_create(&logThread,&attr,logFunc,&command))){
					fprintf(stderr,"Error: Could not create thread: %d\n",rc);
				}
				
				pthread_attr_destroy(&attr);
			}
			
			if(compareText(cmd,"Email")){
				if(pars < 3){  //Need at least the email address and a subject
					printf("Error: Need 3 parameters: address, subject and message\n");
					continue;
				}
				
				command.cmd = cmd;
				command.par[0] = PARAMETERS[0];
				command.par[1] = PARAMETERS[1];
				command.par[2] = PARAMETERS[2];
				
				//=======Call logFunc in thread======
				int rc;
				pthread_attr_t attr;
				pthread_attr_init(&attr);
				pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
				
				if((rc = pthread_create(&emailThread,&attr,emailFunc,&command))){
					fprintf(stderr,"Error: Could not create thread: %d\n",rc);
				}
				
				pthread_attr_destroy(&attr);
			}
			
			if(compareText(cmd,"Webcam")){
				command.cmd = cmd;
				command.par[0] = PARAMETERS[0];

				//=======Call debugFunc in thread======
				int rc;
				pthread_attr_t attr;
				pthread_attr_init(&attr);
				pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
				
				if((rc = pthread_create(&webcamThread,&attr,webcamFunc,&command))){
					fprintf(stderr,"Error: Could not create webcam thread: %d\n",rc);
				}
				
				pthread_attr_destroy(&attr);				
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

	//Free reserved memory
	free((void*)cmd);
	free((void*)par);

	//Close serial's file descriptor
	close(fd);	
}
