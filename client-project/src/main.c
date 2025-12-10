/*
 * main.c   CLIENT
 *
 * UDP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP client
 * portable across Windows, Linux, and macOS.
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
typedef unsigned int socklen_t;
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "protocol.h"

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void usage (const char* progname)
	{
	printf("Uso: %s [-s server] [-p port] -r \"type city\"\n",progname);
	}

void errorhandler(char *errorMessage)
{
	printf("%s", errorMessage);
}

int main(int argc, char *argv[])
{

	//Implement client logic
	char serverName[256]="localhost";
	int port=SERVER_PORT;
	weather_response_t resp;//struct risposta al server
	memset(&resp, 0, sizeof(weather_response_t));
	weather_request_t req;//struct di richiesta al client
	memset(&req,0,sizeof(weather_request_t));


#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif


	for (int i=0;i<argc;i++)
	{
		if(strcmp(argv[i],"-s")==0 && i+1<argc)
		{
			strcpy(serverName,argv[++i]);
		}

		else if(strcmp(argv[i],"-p")==0 && i+1<argc)
		{
			port=atoi(argv[++i]);
		}
		else if(strcmp(argv[i],"-r")==0 && i+1<argc)
		{
			if (sscanf(argv[++i]," %c %63s", &req.type, req.city) !=2)
			{
				usage(argv[0]);
				return 1;
			}
			for (int j=0;req.city[j];++j)
			{
				req.city[j]=tolower((unsigned char)(req.city[j]));
			}

			req.city[0]=toupper((unsigned char)(req.city[0]));
		}
	}

	if(req.type ==0 || strlen(req.city)==0)
	{
		usage(argv[0]);
		return 1;
	}

	//dns
	struct hostent *host;
	struct in_addr addr;
	char ip[16];
	char name[256];

	if(inet_addr(serverName)!=INADDR_NONE)
	{
		addr.s_addr=inet_addr(serverName);
		strcpy(ip, serverName);
		host=gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
		strcpy(name,(host)?host->h_name:serverName);
	}
	else
	{
		host=gethostbyname(serverName);
		if(!host)
		{
			printf("Errore DNS\n");
			return -1;
		}
		strcpy(name,host->h_name);
		addr=*((struct in_addr*)host->h_addr_list[0]);
		strcpy(ip,inet_ntoa(addr));
	}
	//Create socket
	int c_socket;

	c_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(c_socket<0)
	{
		printf("Creazione della socket cliente fallita.\n");
		return 1;
	}

	//Configure server address
	struct sockaddr_in reqs;
	memset(&reqs, 0, sizeof(reqs));
	reqs.sin_family = AF_INET;
	reqs.sin_addr = addr; // IP del server
	reqs.sin_port = htons(port); // Server port
	//send request
	char request_buffer [BUFFER_SIZE];
	memcpy(request_buffer,&req.type,1);
	memcpy(request_buffer+1, req.city,64);

	sendto(c_socket,(void*)request_buffer,sizeof(request_buffer),0,(struct sockaddr*)&reqs,sizeof(reqs));

	//response
	char response_buffer[BUFFER_SIZE];
	struct sockaddr_in clnt;
	int len = sizeof(clnt);
	if(recvfrom(c_socket,(void*)response_buffer,BUFFER_SIZE,0,(struct sockaddr*)&clnt,(socklen_t*)&len)<=0)
	{
		printf("Errore ricezione.\n");
		return 1;
	}

	int offset=0;
	uint32_t status, temp;
	memcpy(&status,&response_buffer+offset,sizeof(uint32_t));
	resp.status=ntohl(status);
	offset+=sizeof(uint32_t);
	memcpy(&resp.type,response_buffer+offset,1);
	offset++;
	memcpy(&temp,response_buffer+offset,sizeof(temp));
	temp=ntohl(temp);
	memcpy(&resp.value,&temp,sizeof(float));


	//status response control
	if (resp.status==0)
	{
		switch (resp.type)
		{
			case 't':
				printf("Ricevuto risultato dal server %s (ip %s). %s: Temperatura = %.1f%cC\n",name,ip,req.city, resp.value,248);
			break;

			case 'h':
				printf("Ricevuto risultato dal server %s (ip %s). %s: Umidita' = %.1f%% \n", name,ip, req.city, resp.value);
				break;

			case 'w':
				printf("Ricevuto risultato dal server %s (ip %s). %s: Vento = %.1f km/h\n", name,ip,req.city, resp.value);
				break;

			case 'p':
				printf("Ricevuto risultato dal server %s (ip %s). %s: Pressione = %.1f hPa\n", name,ip,req.city, resp.value);
				break;
		}
	}
	else if(resp.status==1)
	{
		printf("Ricevuto risultato dal server %s(ip %s). Citta' non disponibile\n",name,ip);
	}
	else if (resp.status==2)
	{
		printf("Ricevuto risultato dal server %s (ip %s). Richiesta non valida\n",name,ip);
	}

	//Close socket
	closesocket(c_socket);
	printf("Client terminato.\n");
	clearwinsock();
	return 0;
} // main end
