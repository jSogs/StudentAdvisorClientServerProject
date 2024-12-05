#include "unp.h"

using namespace std;

void studentLogin(int);
void studentSignup(int);
void checkGrades(int);
void registerClasses(int);
void payBills(int);

string username, password, fullname;

void mainMenu(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE]; // buffers for sending and receiving messages
	char choice[5];
	fputs("Do you have an account?\n1. Yes\n2. No\n", stdout);
	fgets(choice, sizeof(choice), fp);
	choice[strlen(choice) - 1] = '\0';
	int option = atoi(choice);
	if (option == 2)
	{
		studentSignup(sockfd);
	}
	else
	{
		fputs("\nYou can login\n", stdout);
	}
	studentLogin(sockfd);

	fputs("What would you like to do?\n1. Register for classes\n2. Check my grades\n3. Pay my bill\n4. Exit\n", stdout);

	while (fgets(choice, sizeof(choice), fp) != NULL)
	{
		choice[strlen(choice) - 1] = '\0';
		option = atoi(choice);
		switch (option)
		{
		case 1:
			registerClasses(sockfd);
			break;
		case 2:
			checkGrades(sockfd);
			break;
		case 3:
			payBills(sockfd);
			break;
		case 4:
			exit(0);
		default:
			cout << "Invalid option.\n";
			break;
		}
		fputs("What would you like to do?\n1. Register for classes\n2. Check my grades\n3. Pay my bill\n4. Exit\n", stdout);
	}
}

void studentSignup(int sockfd)
{
	fputs("Signup\n", stdout);
	bool signedUp = false;
	do
	{
		char res[MAXLINE];
		string user_name, pass_word, full_name;
		cout << "Enter username: ";
		cin >> user_name;
		cout << "Enter password: ";
		cin >> pass_word;
		cout << "Enter your full name: ";
		cin.ignore();
		getline(cin, full_name);
		string req = "SIGNUP|student|" + user_name + "|" + pass_word + "|" + full_name;
		const char *sendReq = req.c_str();
		write(sockfd, sendReq, strlen(sendReq));
		memset(res, 0, MAXLINE);
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		string resString(res);
		vector<string> parts = split(resString, "|");
		string signup = parts[0];
		string msg = parts[1];
		if (signup == "SUCCESS")
		{
			signedUp = true;
			cout << msg << endl;
		}
		else
		{
			cout << msg << endl;
			continue;
		}
	} while (!signedUp);
}

void studentLogin(int sockfd)
{
	bool signedIn = false;
	do
	{
		char res[MAXLINE];
		cout << "Enter username: ";
		cin >> username;
		cout << "Enter password: ";
		cin >> password;
		string req = "LOGIN|student|" + username + "|" + password;
		cin.ignore();
		const char *sendReq = req.c_str();
		write(sockfd, sendReq, strlen(sendReq));
		memset(res, 0, MAXLINE);
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		string resString(res);
		vector<string> parts = split(resString, "|");
		string signin = parts[0];

		string msg = parts[2]; // store msg from server
		fullname = parts[1];   // store user full_name after successfully login
		if (signin == "SUCCESS")
		{
			signedIn = true;
			cout << msg << endl;
		}
		else
		{
			cout << msg << endl;
			continue;
		}
	} while (!signedIn);
}

void checkGrades(int sockfd)
{
	bool hasChecked = false;
	do
	{
		const char *req;
		char res[MAXLINE];
		string reqString = "GRADES|" + username;
		req = reqString.c_str();
		write(sockfd, req, strlen(req));
		memset(res, 0, MAXLINE);
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		string resString(res);
		resString = *(new string(res));
		vector<string> parts = split(resString, "|");
		string signup = parts[0];
		string msg = parts[1];
		if (signup == "SUCCESS")
		{
			hasChecked = true;
			cout << msg << endl;
		}
		else
		{
			cout << msg << endl;
			continue;
		}
	} while (!hasChecked);
}

void registerClasses(int sockfd)
{
	bool hasRegistered = false;
	do
	{
		const char *req;
		char res[MAXLINE];
		char choices[21];
		string reqString = "CLASSES|" + username;
		req = reqString.c_str();
		write(sockfd, req, strlen(req));
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		string resString(res);
		vector<string> classes = split(resString, "|");
		fputs("Available classes: ", stdout);
		for (int i = 0; i < classes.size(); i++)
		{
			cout << i + 1 << ". " << classes[i] << "\t";
		}
		cout << "\nEnter the number (1-10) and a space between each class you want to register for: ";
		fgets(choices, sizeof(choices), stdin);
		write(sockfd, choices, strlen(choices));
		memset(res, 0, MAXLINE);
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		resString = *(new string(res));
		vector<string> parts = split(resString, "|");
		string signup = parts[0];
		string msg = parts[1];
		if (signup == "SUCCESS")
		{
			hasRegistered = true;
			cout << msg << endl;
		}
		else
		{
			cout << msg << endl;
			continue;
		}
	} while (!hasRegistered);
}

void payBills(int sockfd)
{
	bool hasPay = false;
	do {
		const char * req;
		char res[MAXLINE];
		string reqString = "BILL|" + username;
		req = reqString.c_str();
		write(sockfd, req, strlen(req));
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		string resString(res);
		vector<string> parts = split(resString, "|");
		if(parts[0] == "FAILURE") {
			cout << parts[1];
			return;
		}

		// print the amount send by the server
		cout << resString;

		// take input for how much user want to pay
		cin >> reqString;
		req = reqString.c_str();
		write(sockfd, req, strlen(req));
		cin.ignore();

		// check if pay is success
		memset(res, 0, MAXLINE); // clear buffer
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		resString = res;
		parts = split(resString, "|");
		if(parts[0] == "FAILURE") {
			cout << parts[1] << endl;
			return;
		}
		else {
			hasPay = true;
			cout << parts[1] << endl;
		}
	} while(!hasPay);
}
