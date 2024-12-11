#include "unp.h"

using namespace std;

// Function Headers
void studentLogin(int);
void studentSignup(int);
void checkGrades(int);
void registerClasses(int);
void payBills(int);
void vieworDropClasses(int);

string username, password, fullname; // store user detail on local

void mainMenu(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE]; // buffers for sending and receiving messages
	char choice[5];

	// Ask if user has an account
	fputs("Do you have an account?\n1. Yes\n2. No\n", stdout);
	fgets(choice, sizeof(choice), fp);
	choice[strlen(choice) - 1] = '\0';
	int option = atoi(choice);
	if (option == 2) // if no account then sign up
	{
		studentSignup(sockfd);
	}
	else // otherwise user can login
	{
		fputs("\nYou can login\n", stdout);
	}
	studentLogin(sockfd);

	// Show menu after successfully login
	fputs("What would you like to do?\n1. Register for classes\n2. Check my grades\n3. Pay my bill\n4. View or Drop Classes\n5. Exit\n", stdout);

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
			vieworDropClasses(sockfd);
			break;
		case 5:
			exit(0);
		default:
			cout << "Invalid option.\n";
			break;
		}
		fputs("What would you like to do?\n1. Register for classes\n2. Check my grades\n3. Pay my bill\n4. View or Drop Classes\n5. Exit\n", stdout);
	}
}

// Handle Signup 
void studentSignup(int sockfd)
{
	fputs("Signup\n", stdout);
	bool signedUp = false;
	do
	{
		// Ask user to provide their detail for creating account
		char res[MAXLINE];
		string user_name, pass_word, full_name;
		cout << "Enter username: ";
		cin >> user_name;
		cout << "Enter password: ";
		cin >> pass_word;
		cout << "Enter your full name: ";
		cin.ignore();
		getline(cin, full_name);

		// Send detail to server
		string req = "SIGNUP|student|" + user_name + "|" + pass_word + "|" + full_name;
		const char *sendReq = req.c_str();
		write(sockfd, sendReq, strlen(sendReq));

		// Read response from server
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

// Handle login
void studentLogin(int sockfd)
{
	bool signedIn = false;
	do
	{
		// Get detail for login
		char res[MAXLINE];
		cout << "Enter username: ";
		cin >> username;
		cout << "Enter password: ";
		cin >> password;

		// Send account detail to server
		string req = "LOGIN|student|" + username + "|" + password;
		cin.ignore();
		const char *sendReq = req.c_str();
		write(sockfd, sendReq, strlen(sendReq));

		// Read response from server
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
			cout << "\n" << msg << endl;
		}
		else
		{
			cout << msg << endl;
			continue;
		}
	} while (!signedIn);
}

// Handle Check Grade
void checkGrades(int sockfd)
{
	// Send request to server
	const char *req;
	char res[MAXLINE];
	string reqString = "GRADES|" + username;
	req = reqString.c_str();
	write(sockfd, req, strlen(req));

	// Get response from server
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
		cout << msg << endl;
	}
	else
	{
		cout << msg << endl;
	}
}

// Handle Register
void registerClasses(int sockfd)
{
	bool hasRegistered = false;
	do
	{
		// Send request to server
		const char *req;
		char res[MAXLINE];
		char choices[21];
		string reqString = "CLASSES|" + username;
		req = reqString.c_str();
		write(sockfd, req, strlen(req));

		// Read response from server
		memset(res, 0, MAXLINE);
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}

		// show available classes
		string resString(res);
		vector<string> classes = split(resString, "|");
		fputs("Available classes: \n", stdout);
		for (int i = 0; i < classes.size(); i++)
		{
			cout << i + 1 << ". " << classes[i] << "\n";
		}
		cout << "\nEnter the number (1-10) and a space between each class you want to register for: ";

		// ask what classes does user want and send that request
		fgets(choices, sizeof(choices), stdin);
		write(sockfd, choices, strlen(choices));

		// Read response from server
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

// Handle pay bill
void payBills(int sockfd)
{
	bool hasPay = false;
	do
	{
		// Send request to server
		const char *req;
		char res[MAXLINE];
		string reqString = "BILL|" + username;
		req = reqString.c_str();
		write(sockfd, req, strlen(req));

		// Read response from server
		memset(res,0,MAXLINE);
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}
		string resString(res);
		vector<string> parts = split(resString, "|");
		if (parts[0] == "FAILURE") // error check
		{
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
		if (parts[0] == "FAILURE")
		{
			cout << parts[1] << endl;
			return;
		}
		else
		{
			hasPay = true;
			cout << parts[1] << endl;
		}
	} while (!hasPay);
}

// Handle view or drop classes
void vieworDropClasses(int sockfd)
{
	bool hasViewed = false;
	do
	{
		// Send request to server
		const char *req;
		char res[MAXLINE];
		char choices[21];
		string reqString = "VIEW|" + username;
		req = reqString.c_str();
		write(sockfd, req, strlen(req));

		// Read response from server
		memset(res, 0, MAXLINE);
		if (read(sockfd, res, MAXLINE) == 0)
		{ // read from socket
			printf("str_cli: server terminated prematurely");
			exit(1);
		}

		// Show classes that the user currently take
		string resString(res);
		vector<string> classes = split(resString, "|");
		fputs("Available classes: \n", stdout);
		for (int i = 1; i < classes.size(); i++)
		{
			cout << i << ". " << classes[i] << "\n";
		}
		if (classes.size() <= 1)
		{
			reqString = "0";
			req = reqString.c_str();
			write(sockfd, req, strlen(req));
			read(sockfd, res, MAXLINE);
			return;
		}

		// Ask if user want to drop any classes
		cout << "\nEnter the number of all classes you want to drop or 0 if you don't want to drop any:";
		fgets(choices, sizeof(choices), stdin);
		choices[strlen(choices) - 1] = '\0';
		hasViewed = true;

		// Send request to server
		write(sockfd, choices, strlen(choices));

		// Read response from server
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
			hasViewed = true;
			cout << msg << endl;
		}
		else
		{
			cout << msg << endl;
			continue;
		}
	} while (!hasViewed);
}
