#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <mutex>
#define MAX_LEN 200
#define NUM_COLORS 6
#define key 5

using namespace std;

bool exit_flag=false;
thread t_send, t_recv;
int client_socket;
string def_col="\033[0m";
string colors[]={"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};

void exit_terminal(int signal);
string color(int code);
int eraseText(int cnt);
void send_message(int client_socket);
void recv_message(int client_socket);
void Encrypt(char message[]);
void Decrypt(char message[]);

int main()
{
	if((client_socket=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_port=htons(10000);
	client.sin_addr.s_addr=INADDR_ANY;
	bzero(&client.sin_zero,0);

	if((connect(client_socket,(struct sockaddr *)&client,sizeof(struct sockaddr_in)))==-1)
	{
		perror("connect: ");
		exit(-1);
	}
	signal(SIGINT, exit_terminal);
	int x;
	char name[MAX_LEN],password[MAX_LEN];
	cout<<"Enter your name : ";
	cin.getline(name,MAX_LEN);
	fflush(stdin);
	cout<<"Enter your passcode : ";
	cin.getline(password,MAX_LEN);
	send(client_socket,name,sizeof(name),0);
	send(client_socket,password,sizeof(password),0);
	do
	{
		recv(client_socket,&x,sizeof(x),0);
		if(x==0)
			break;
		cout<<"Invalid Passcode, Please Enter Again : ";
		cin.getline(password,MAX_LEN);
		send(client_socket,password,sizeof(password),0);
	}while(1);
	cout<<colors[NUM_COLORS-1]<<"\n\t  ~ ~ ~ ~ MetaChat ~ ~ ~ ~   "<<endl<<def_col;

	thread t1(send_message, client_socket);
	thread t2(recv_message, client_socket);

	t_send=move(t1);
	t_recv=move(t2);

	if(t_send.joinable())
		t_send.join();
	if(t_recv.joinable())
		t_recv.join();
			
	return 0;
}

void exit_terminal(int signal) 
{
	char str[MAX_LEN]="#exit";
	send(client_socket,str,sizeof(str),0);
	exit_flag=true;
	t_send.detach();
	t_recv.detach();
	close(client_socket);
	exit(signal);
}

string color(int code)
{
	return colors[code%NUM_COLORS];
}

int eraseText(int cnt)
{
	char back_space=8;
	for(int i=0; i<cnt; i++)
	{
		cout<<back_space;
	}	
}

void send_message(int client_socket)
{
	while(1)
	{
		cout<<colors[1]<<"You : "<<def_col;
		char str[MAX_LEN],rev[MAX_LEN];
		int temp[MAX_LEN];
		fflush(stdin);
        	cin>>rev;
		fflush(stdin);
		cin.getline(str-1,MAX_LEN+1);
		Encrypt(str);
        	send(client_socket,rev,sizeof(rev),0);
		if(strcmp(rev,"#exit")==0)
		{
			exit_flag=true;
			t_recv.detach();	
			close(client_socket);
			return;
		}
		send(client_socket,str,sizeof(str),0);	
	}		
}

void recv_message(int client_socket)
{
	while(1)
	{
		if(exit_flag)
			return;
		char name[MAX_LEN], str[MAX_LEN];
		int temp[MAX_LEN];
		int color_code;
		int bytes_received=recv(client_socket,name,sizeof(name),0);
		if(bytes_received<=0)
			continue;
		recv(client_socket,&color_code,sizeof(color_code),0);
		recv(client_socket,str,sizeof(str),0);
		eraseText(6);
		if(strcmp(name,"#NULL")!=0)
		{
			Decrypt(str);
			cout<<color(color_code)<<name<<" : "<<def_col<<str<<endl;
		}
		else
			cout<<color(color_code)<<str<<endl;
		cout<<colors[1]<<"You : "<<def_col;
		fflush(stdout);
	}	
}

void Encrypt(char message[])
{
	int i;
	for(i=0;message[i]!='\0';i++)
	{
		message[i]=~(message[i]+key);
	}
}

void Decrypt(char message[])
{
	int i;
	for(i=0;message[i]!=0;i++)
	{
		message[i]=~message[i]-key;
	}
}
