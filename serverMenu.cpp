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

mongocxx::instance instance{}; //Mongodb singleton instance
const string MONGODB_CONNECTION_STRING = "mongodb+srv://julianasogwa96:o5zKPuqoOnAup7PJ@cluster0.kl71a.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0";

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
        cout<<req<<endl;
		string request(req);
        vector<string> parts = split(request,"|");
		string requestType = parts[0];
		if(requestType == "LOGIN") handleLogin(parts,sockfd);
		else if(requestType == "SIGNUP") handleSignup(parts,sockfd);
		else if(requestType == "GRADES") handleCheckGrades(parts,sockfd);
		else if(requestType == "CLASSES") handleRegisterClasses(parts,sockfd);
		else if(requestType == "BILL") handlePayBill(parts,sockfd);
		memset(req, 0, MAXLINE);
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

	mongocxx::uri uri(MONGODB_CONNECTION_STRING);
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
		mongocxx::uri uri(MONGODB_CONNECTION_STRING);
		mongocxx::client client{uri};

		auto database = client["main"];          // Access the database
		auto collection = database["users"];    // Access a collection

		auto result = collection.find_one(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("username", username)));
		//if username exist
		if(result) {
			auto doc = *result;
            string stored_password = doc["password"].get_string().value.to_string();

            // Compare passwords
            if (stored_password == password) {
				string full_name = doc["full_name"].get_string().value.to_string();
				string resString = "SUCCESS|" + full_name + "|Welcome " + username + ". Login successful.\n";
				const char* res = resString.c_str();
                write(sockfd, res, strlen(res));
            } else {
				string resString = "FAILURE|null|Failed to sign in. Try again.";
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
	ssize_t	n;
	string username = parts[1];
	char req[MAXLINE];
	string classListStr = "MATH 2215|GEOG 3003|CHEM 1113|CS 2413|CS 2513|CS 3013|BIO 2113|MATH 3013|HIST 1495|SOCI 1003";
	const char* classList = classListStr.c_str();
	vector<string> classes = split(classListStr,"|");
	write(sockfd, classList, strlen(classList));
	while((n = read(sockfd, req, MAXLINE)) <= 0){ //read from socket
		if (n < 0 && errno == EINTR) //if process is interrupted retry
			continue;
		else if (n < 0){
			printf("server: read error");
			exit(1);
		}
	}
	string classChoices(req);
	vector<string> choices = split(classChoices," ");
	cout<<req<<endl;

	mongocxx::uri uri(MONGODB_CONNECTION_STRING);
	mongocxx::client client{uri}; // Connect to MongoDB
	
	auto database = client["main"];          // Access the database
	auto collection = database["users"];    // Access a collection

	cout << "\nSuccessfully connected to MongoDB!" << endl;

	// Filter to find the user
    bsoncxx::builder::stream::document filter_builder{};
    filter_builder << "username" << username;

	// Create the array of classes to push
	bsoncxx::builder::basic::array class_array;

	for (int i = 0; i < choices.size(); i++) {
		int choice = atoi(choices[i].c_str());
		class_array.append(bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("name", classes[choice - 1])
		));
		std::cout << "Added to class_array: { \"name\": \"" << classes[choice - 1] << "\" }" << std::endl;
	}

	// Print the array
	std::cout << "Final Class Array: " << bsoncxx::to_json(class_array.view()) << std::endl;


	// Build the update document
    bsoncxx::builder::stream::document update_builder{};
    update_builder << "$push" << bsoncxx::builder::stream::open_document
                   << "classes" << bsoncxx::builder::stream::open_document
                   << "$each" << class_array.view()
                   << bsoncxx::builder::stream::close_document
                   << bsoncxx::builder::stream::close_document;

	std::cout << "Update Query: " << bsoncxx::to_json(update_builder.view()) << std::endl;

	try {
        auto result = collection.update_one(filter_builder.view(), update_builder.view());
        if (result && result->modified_count() > 0) {
            cout << "Classes added successfully." << std::endl;
			string resString="SUCCESS|You have successfully registered for: \n";
			for (int i = 0; i < choices.size(); i++) {
				int choice = atoi(choices[i].c_str());
				resString+=classes[choice-1]+"\n";
			}
			const char* res = resString.c_str();
			write(sockfd, res, strlen(res));
        } else {
			string resString="FAILURE|Failed to register for classes. Try again.";
			const char* res = resString.c_str();
			write(sockfd, res, strlen(res));
            cout << "No documents updated." << std::endl;
        }
    } catch (const std::exception& e) {
		string resString="FAILURE|Failed to register for classes. Try again.";
		const char* res = resString.c_str();
		write(sockfd, res, strlen(res));
        cerr << "Update error: " << e.what() << std::endl;
    }
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
