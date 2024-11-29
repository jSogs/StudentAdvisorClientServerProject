#include "unp.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

void studentLogin(int);
void studentSignup(int);
void checkGrades();
void registerClasses();
void payBills();

vector<string> split(const string&, const string&);
string username, password, fullname;

void mainMenu(FILE *fp, int sockfd){
	char sendline[MAXLINE], recvline[MAXLINE]; //buffers for sending and receiving messages
	char choice[5];
	fputs("Do you have an account?\n1. Yes\n2. No\n",stdout);
	fgets(choice, sizeof(choice), fp);
	choice[strlen(choice)-1] = 0;
	int option = atoi(choice);
	if(option==2){
		studentSignup(sockfd);
	} else{
		fputs("You can login\n", stdout);
	}
	studentLogin(sockfd);

	while (fgets(choice, sizeof(choice), fp) != NULL) { 
		fputs("What would you like to do?\n1. Register for classes\n2. Check my grades\n3. Pay my bill\n4. Exit\n",stdout);
		fgets(choice, sizeof(choice), fp);
		choice[strlen(choice)-1] = 0;
		option = atoi(choice);
		switch(option) {
			case 1: registerClasses(); break;
			case 2: checkGrades(); break;
			case 3: payBills(); break;
			case 4: exit(0); break;
			default: cout << "Invalid option.\n"; break;
		}
		
	}
}

void studentSignup(int sockfd){
	bool signedUp = false;
	do
	{
		char res[MAXLINE];
		string user_name, pass_word, full_name;
		cout << "Enter username: ";
		cin >> user_name;
		cout <<"Enter password: ";
		cin >> pass_word;
		cout <<"Enter your full name: ";
		cin.ignore();
		getline(cin,full_name);
		string req = "SIGNUP|student|"+user_name+"|"+pass_word+"|"+full_name;
		const char* sendReq = req.c_str();
		write(sockfd, sendReq, strlen(sendReq));
		if (read(sockfd, res, MAXLINE) == 0){ //read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		string resString(res);
		vector<string> parts = split(resString,"|");
		string signup = parts[0];
		string msg = parts[1];
		if(signup == "SUCCESS"){
			signedUp = true;
			cout<<msg<<endl;
		} else {
			cout<<msg<<endl;
			continue;
		}
	} while (!signedUp);
	
}

void studentLogin(int sockfd){
	bool signedIn = false;
	do
	{
		char res[MAXLINE];
		cout << "Enter username: ";
		cin >> username;
		cout <<"Enter password: ";
		cin >> password;
		string req = "LOGIN|student|"+username+"|"+password;
		const char* sendReq = req.c_str();
		write(sockfd, sendReq, strlen(sendReq));
		if (read(sockfd, res, MAXLINE) == 0){ //read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		string resString(res);
		vector<string> parts = split(resString,"|");
		string signin = parts[0];

		string msg = parts[2]; // store msg from server
		string full_name = parts[1]; // store user full_name after successfully login
		cout << full_name << endl;

		if(signin == "SUCCESS"){
			signedIn = true;
			cout<<msg<<endl;
		} else {
			cout<<msg<<endl;
			continue;
		}
	} while (!signedIn);
}

void checkGrades(){

}

void registerClasses(){

}

void payBills(){

}

vector<string> split(const string& str, const string& delimiter){
    vector<string> tokens;
	string::size_type start = 0;
	string:: size_type end = str.find(delimiter);

	while(end!= string::npos){
		tokens.push_back(str.substr(start, end-start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}
	tokens.push_back(str.substr(start));
	return tokens;
}
