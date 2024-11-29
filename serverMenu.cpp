#include "unp.h"
#include <vector>
#include <string>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using namespace std;

vector<string> split(const string&, const string&);
void handleSignup(const vector<string>&, int);
void handleLogin(const vector<string>&, int);
void handleRegisterClasses(const vector<string>&, int);
void handleCheckGrades(const vector<string>&, int);
void handlePayBill(const vector<string>&, int);

void serverMenu(int sockfd){
	ssize_t	n;
	char sendline[MAXLINE], recvline[MAXLINE]; //buffers for sending and receiving messages
	char req[MAXLINE];
again:
	while ( (n = read(sockfd, req, MAXLINE)) > 0){ //read data from socket
        fputs(req, stdout);
		string request(req);
        vector<string> parts = split(request,"|");
		string requestType = parts[0];
		if(requestType == "LOGIN") handleLogin(parts,sockfd);
		else if(requestType == "SIGNUP") handleSignup(parts,sockfd);
		else if(requestType == "GRADES") handleCheckGrades(parts,sockfd);
		else if(requestType == "CLASSES") handleRegisterClasses(parts,sockfd);
		else if(requestType == "BILL") handlePayBill(parts,sockfd);
	}

	if (n < 0 && errno == EINTR) //if process is interrupted retry
		goto again;
	else if (n < 0){
		printf("server: read error");
		exit(1);
	}
}

void handleSignup(const vector<string> &parts, int sockfd){
	string role = parts[1];
	string username = parts[2];
	string password = parts[3];
	string full_name = parts[4];

    mongocxx::instance instance{};
	mongocxx::uri uri("mongodb+srv://julianasogwa96:o5zKPuqoOnAup7PJ@cluster0.kl71a.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0");
	mongocxx::client client{uri}; // Connect to MongoDB
	
	auto database = client["main"];          // Access the database
	auto collection = database["users"];    // Access a collection

	cout << "\nSuccessfully connected to MongoDB!" << endl;

	//Create a mongodb document
	bsoncxx::builder::stream::document document{};
	document<<"role"<<role
			<<"username"<<username
			<<"password"<<password
			<<"full_name"<<full_name
			<<"classes"<<bsoncxx::builder::stream::array{};
	
	try{
        // Perform operations like find or insert (example)
        auto result = collection.insert_one(document.view());
        if(result){
			string resString="SUCCESS|"+username+" is a valid account. You can now signin\n";
			const char* res = resString.c_str();
			write(sockfd, res, strlen(res));
		}
    } catch (const std::exception& e) {
		string resString="FAILURE|Failed to sign up. Try again.";
		const char* res = resString.c_str();
		write(sockfd, res, strlen(res));
        std::cerr << "Connection error: " << e.what() << std::endl;
    }
}

void handleLogin(const vector<string> &parts, int sockfd){
	string role = parts[1];
	string username = parts[2];
	string password = parts[3];

	// search if account exist
	try {
	// connect to MongoDB
		mongocxx::instance instance{};
		mongocxx::uri uri("mongodb+srv://julianasogwa96:o5zKPuqoOnAup7PJ@cluster0.kl71a.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0");
		mongocxx::client client{uri};

		auto database = client["main"];          // Access the database
		auto collection = database["users"];    // Access a collection

		auto result = collection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
		//if username exist
		if(result) {
			auto doc = *result;
            string stored_password = doc["password"].get_string().value.to_string();

            // Compare passwords
            if (stored_password == password) { // Replace with actual hash comparison logic
				string resString = "SUCCESS|"+username+" is a valid account. Login successful.\n";
				const char* res = resString.c_str();
                write(sockfd, res, strlen(res));
            } else {
				string resString = "FAILURE|Failed to sign in. Try again.";
				const char* res = resString.c_str();
                write(sockfd, res, strlen(res));
            }
		}
	}
	catch (const std::exception& e) {
		cout << "Account does not exist\n";
	}

}

void handleRegisterClasses(const vector<string> &parts, int sockfd){

}

void handleCheckGrades(const vector<string> &parts, int sockfd){

}

void handlePayBill(const vector<string> &parts, int sockfd){

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
