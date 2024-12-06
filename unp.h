#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<sys/wait.h>
#include    <string>
#include 	<set>
#include    <iostream>
#include    <vector>
#define	LISTENQ		1024	/* 2nd argument to listen() */
#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define	SERV_PORT	 9877			/* TCP and UDP */
#define	SERV_PORT_STR	"9877"			/* TCP and UDP */
#define	SA	struct sockaddr
void mainMenu(FILE *,int);
void serverMenu(int);

inline std::vector<std::string> split(const std::string& str, const std::string& delimiter){
    std::vector<std::string> tokens;
	std::string::size_type start = 0;
	std::string:: size_type end = str.find(delimiter);

	while(end!= std::string::npos){
		tokens.push_back(str.substr(start, end-start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}
	tokens.push_back(str.substr(start));
	return tokens;
}

inline std::string generateGrade(){
	int num = rand()%51;
	if(num>49) return "A+";
	else if(num>45) return "A";
	else if(num>40) return "A-";
	else if(num>38) return "B+";
	else if(num>33) return "B";
	else if(num>30) return "B-";
	else if(num>20) return "C";
	else if(num>10) return "D";
	else return "F";
}
