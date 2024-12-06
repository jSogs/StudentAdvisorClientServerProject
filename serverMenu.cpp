#include "unp.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using namespace std;

mongocxx::instance instance{}; // mongodb singleton instance
const string MONGODB_CONNECTION_STRING = "mongodb+srv://julianasogwa96:o5zKPuqoOnAup7PJ@cluster0.kl71a.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0";

void handleSignup(const vector<string> &, int);
void handleLogin(const vector<string> &, int);
void handleRegisterClasses(const vector<string> &, int);
void handleCheckGrades(const vector<string> &, int);
void handlePayBill(const vector<string> &, int);
void handleShowClasses(const vector<string> &, int);

void serverMenu(int sockfd)
{
	srand(time(NULL));
	ssize_t n;
	char sendline[MAXLINE], recvline[MAXLINE]; // buffers for sending and receiving messages
	char req[MAXLINE];
again:
	while ((n = read(sockfd, req, MAXLINE)) > 0)
	{ // read data from socket
		cout << req << endl;
		string request(req);
		vector<string> parts = split(request, "|");
		string requestType = parts[0];
		if (requestType == "LOGIN")
			handleLogin(parts, sockfd);
		else if (requestType == "SIGNUP")
			handleSignup(parts, sockfd);
		else if (requestType == "GRADES")
			handleCheckGrades(parts, sockfd);
		else if (requestType == "CLASSES")
			handleRegisterClasses(parts, sockfd);
		else if (requestType == "BILL")
			handlePayBill(parts, sockfd);
		else if (requestType == "");
		memset(req, 0, MAXLINE);
	}

	if (n < 0 && errno == EINTR) // if process is interrupted retry
		goto again;
	else if (n < 0)
	{
		printf("server: read error");
		exit(1);
	}
}

void handleSignup(const vector<string> &parts, int sockfd)
{
	// Get user details
	string role = parts[1];
	string username = parts[2];
	string password = parts[3];
	string full_name = parts[4];

	// Connect to MongoDB
	mongocxx::uri uri(MONGODB_CONNECTION_STRING);
	mongocxx::client client{uri};

	auto database = client["main"];		 // Access the database
	auto collection = database["users"]; // Access a collection

	cout << "\nSuccessfully connected to MongoDB!" << endl;

	// Create a mongodb document
	bsoncxx::builder::stream::document document{};
	document << "role" << role
			 << "username" << username
			 << "password" << password
			 << "full_name" << full_name
			 << "bill" << 0
			 << "classes" << bsoncxx::builder::stream::array{};

	try
	{
		// Insert the new document into the database
		auto result = collection.insert_one(document.view());
		if (result)
		{ // If successfully created send a SUCCESS response to the client
			string resString = "SUCCESS|" + username + " is a valid account. You can now signin\n";
			const char *res = resString.c_str();
			write(sockfd, res, strlen(res));
		}
	}
	catch (const std::exception &e)
	{
		// If any error send a FAILURE response to the client
		string resString = "FAILURE|Failed to sign up. Try again.";
		const char *res = resString.c_str();
		write(sockfd, res, strlen(res));
		std::cerr << "Connection error: " << e.what() << std::endl;
	}
}

void handleLogin(const vector<string> &parts, int sockfd)
{
	string role = parts[1];
	string username = parts[2];
	string password = parts[3];

	// Search if account exist
	try
	{
		// Connect to MongoDB
		mongocxx::uri uri(MONGODB_CONNECTION_STRING);
		mongocxx::client client{uri};

		auto database = client["main"];		 // Access the database
		auto collection = database["users"]; // Access a collection

		auto result = collection.find_one(bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("username", username)));

		// If username exists
		if (result)
		{
			auto doc = *result;
			string stored_password = doc["password"].get_string().value.to_string();

			// Compare passwords
			if (stored_password == password)
			{
				string full_name = doc["full_name"].get_string().value.to_string();
				string resString = "SUCCESS|" + full_name + "Successful login\n" + "|Welcome " + username + "\n";
				const char *res = resString.c_str();
				write(sockfd, res, strlen(res));
			}
			else
			{
				string resString = "FAILURE|null|Failed to sign in. Try again.";
				const char *res = resString.c_str();
				write(sockfd, res, strlen(res));
			}
		}
	}
	catch (const std::exception &e)
	{
		string resString = "FAILURE|null|Failed to sign in. Try again.";
		const char *res = resString.c_str();
		write(sockfd, res, strlen(res));
		cout << "Account does not exist\n";
	}
}

void handleRegisterClasses(const vector<string> &parts, int sockfd)
{
	ssize_t n;
	string username = parts[1];																							  // Get the user name
	char req[MAXLINE];																									  // Request variable
	string classListStr = "MATH 2215|GEOG 3003|CHEM 1113|CS 2413|CS 2513|CS 3013|BIO 2113|MATH 3013|HIST 1495|SOCI 1003"; // List of classes
	const char *classList = classListStr.c_str();
	vector<string> classes = split(classListStr, "|"); // Split class list to array
	write(sockfd, classList, strlen(classList));	   // Send list of classes to the user
	while ((n = read(sockfd, req, MAXLINE)) <= 0)
	{								 // Read from socket
		if (n < 0 && errno == EINTR) // If process is interrupted retry
			continue;
		else if (n < 0)
		{
			printf("server: read error");
			exit(1);
		}
	}
	string classChoices(req);						   // Convert the user choices from cstring to string
	vector<string> choices = split(classChoices, " "); // Split to array
	cout << req << endl;

	// Connect to MongoDB
	mongocxx::uri uri(MONGODB_CONNECTION_STRING);
	mongocxx::client client{uri};

	auto database = client["main"];		 // Access the database
	auto collection = database["users"]; // Access a collection

	cout << "\nSuccessfully connected to MongoDB!" << endl;

	// Filter to find the user
	bsoncxx::builder::stream::document filter_builder{};
	filter_builder << "username" << username;

	// Create the array of classes to push
	bsoncxx::builder::basic::array class_array;

	for (int i = 0; i < choices.size(); i++)
	{
		int choice = atoi(choices[i].c_str());
		class_array.append(bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("name", classes[choice - 1])));
		std::cout << "Added to class_array: { \"name\": \"" << classes[choice - 1] << "\" }" << std::endl;
	}

	// Print the array
	std::cout << "Final Class Array: " << bsoncxx::to_json(class_array.view()) << std::endl;

	// Update the classes field in the database
	//$set operation replaces what was in the classes field previously
	//.view() makes the document readable
	bsoncxx::builder::stream::document update_builder{};
	update_builder << "$set" << bsoncxx::builder::stream::open_document
				   << "classes" << class_array.view()
				   << bsoncxx::builder::stream::close_document;
	// std::cout << "Update Query: " << bsoncxx::to_json(update_builder.view()) << std::endl;

	try
	{
		// Perform database update
		auto result = collection.update_one(filter_builder.view(), update_builder.view());
		if (result && result->modified_count() > 0)
		{
			// If a document was modified, send SUCCESS response to the client with list of registered classes
			cout << "Classes added successfully." << std::endl;
			string resString = "SUCCESS|You have successfully registered for: \n";
			for (int i = 0; i < choices.size(); i++)
			{
				int choice = atoi(choices[i].c_str());
				resString += classes[choice - 1] + "\n";
			}
			const char *res = resString.c_str();
			write(sockfd, res, strlen(res));
		}
		else
		{
			// If did not work, send FAILURE response to the client
			string resString = "FAILURE|Failed to register for classes. Try again.";
			const char *res = resString.c_str();
			write(sockfd, res, strlen(res));
			cout << "No documents updated." << std::endl;
		}
	}
	catch (const std::exception &e)
	{
		// If any error, send FAILURE response to the client
		string resString = "FAILURE|Failed to register for classes. Try again.";
		const char *res = resString.c_str();
		write(sockfd, res, strlen(res));
		cerr << "Update error: " << e.what() << std::endl;
	}
}

void handleCheckGrades(const vector<string> &parts, int sockfd)
{
	string username = parts[1]; // Get username
	string resString = "";

	// Filter to find the user
	bsoncxx::builder::stream::document filter_builder{};
	filter_builder << "username" << username;

	// Connect to MongoDB
	mongocxx::uri uri(MONGODB_CONNECTION_STRING);
	mongocxx::client client{uri};

	auto database = client["main"];		 // Access the database
	auto collection = database["users"]; // Access a collection

	cout << "\nSuccessfully connected to MongoDB!" << endl;

	// Get the user record from the database
	auto result = collection.find_one(filter_builder.view());
	if (!result)
	{
		// If could not find result, send FAILURE response to the client
		cerr << "No user found for username: " << username << endl;
		resString = "FAILURE|Could not get grades";
		const char *res = resString.c_str();
		write(sockfd, res, strlen(res));
		return;
	}

	// Fetch the current classes
	auto doc = *result;
	auto current_classes = doc["classes"].get_array().value;

	// Check if any class already has a grade
	bool has_grades = false;
	for (const auto &class_doc : current_classes)
	{
		if (class_doc["grade"])
		{ // Check if the "grade" field exists
			has_grades = true;
			break;
		}
	}

	// If does not have grades, give grades
	if (!has_grades)
	{
		bsoncxx::builder::basic::array class_array; // Builder array to build class documents

		// Create a grade for eah class
		for (const auto &class_doc : current_classes)
		{
			string grade = generateGrade(); // Call generate random grade function
			// Make doucment and add to builder
			class_array.append(bsoncxx::builder::basic::make_document(
				bsoncxx::builder::basic::kvp("name", class_doc["name"].get_string().value.to_string()), // have to use the whole .string stuff cuz it initially returns a BSON type
				bsoncxx::builder::basic::kvp("grade", grade)));
			cout << "Added to class_array: { \"name\": \"" << class_doc["name"].get_string().value.to_string() << "\" }" << "{ \"grade\": \"" << grade << "\" }" << endl;
		}

		// Update the classes field in the database
		bsoncxx::builder::stream::document update_builder{};
		update_builder << "$set" << bsoncxx::builder::stream::open_document
					   << "classes" << class_array.view()
					   << bsoncxx::builder::stream::close_document;

		try
		{
			auto update_result = collection.update_one(filter_builder.view(), update_builder.view());
			if (update_result && update_result->modified_count() > 0)
			{
				// Re-fetch the updated document
				auto update = collection.find_one(filter_builder.view());
				auto new_doc = *update;
				if (update)
				{
					// Send SUCCESS response to client with classes and grades
					auto classes = new_doc["classes"].get_array().value;
					cout << "Classes updated successfully with grades." << endl;
					resString = "SUCCESS|Your current grades are:\n";
					for (const auto &class_doc : classes)
					{
						resString += class_doc["name"].get_string().value.to_string() + ": " + class_doc["grade"].get_string().value.to_string() + "\n";
					}
					const char *res = resString.c_str();
					write(sockfd, res, strlen(res));
				}
				else
				{
					// Send FAILURE response to client
					cerr << "Failed to fetch updated document." << endl;
					resString = "FAILURE|Could not fetch updated grades.";
					const char *res = resString.c_str();
					write(sockfd, res, strlen(res));
					return;
				}
			}
			else
			{
				// Send FAILURE response to client
				resString = "FAILURE|Could not get grades";
				const char *res = resString.c_str();
				write(sockfd, res, strlen(res));
				cout << "No documents updated. Check if the query matches the document." << endl;
			}
		}
		catch (const std::exception &e)
		{
			// Send FAILURE response to client
			resString = "FAILURE|Could not get grades";
			const char *res = resString.c_str();
			write(sockfd, res, strlen(res));
			cerr << "Update error: " << e.what() << endl;
		}
	}
	else
	{
		// If classes already have grades, no need to generate grades
		// Just send SUCCESS response to the client
		resString = "SUCCESS|Your current grades are:\n";
		for (const auto &class_doc : current_classes)
		{
			resString += class_doc["name"].get_string().value.to_string() + ": " + class_doc["grade"].get_string().value.to_string() + "\n";
		}
		const char *res = resString.c_str();
		cout << res << endl;
		write(sockfd, res, strlen(res));
	}
}

void handlePayBill(const vector<string> &parts, int sockfd)
{
	string username = parts[1];

	// Search if account exist
	try
	{
		// Connect to MongoDB
		mongocxx::uri uri(MONGODB_CONNECTION_STRING);
		mongocxx::client client{uri};

		auto database = client["main"];		 // Access the database
		auto collection = database["users"]; // Access a collection

		auto result = collection.find_one(bsoncxx::builder::basic::make_document(
			bsoncxx::builder::basic::kvp("username", username)));

		// If username exists
		if (result)
		{
			// send amount remaining to user
			auto doc = *result;
			int bill = doc["bill"].get_int32().value;
			string resString = "Amount Remaining: " + to_string(bill) + "\nHow much do you want to pay?\n";
			const char *res = resString.c_str();
			write(sockfd, res, strlen(res));

			// read how much user want to pay
			char res2[MAXLINE];
			if (read(sockfd, res2, MAXLINE) == 0)
			{ // read from socket
				printf("str_cli: server terminated prematurely");
				exit(1);
			}

			// Subtract the remain from how much user want to pay
			string amountPay(res2);
			bill = bill - stoi(amountPay);

			
			// Filter to find the user
			bsoncxx::builder::stream::document filter_builder{};
			filter_builder << "username" << username;

			bsoncxx::builder::stream::document update_builder{};
			update_builder << "$set" << bsoncxx::builder::stream::open_document
				   << "bill" << bill
				   << bsoncxx::builder::stream::close_document;

			// Update the bill for the user after pay
			try {
				auto result2 = collection.update_one(filter_builder.view(), update_builder.view());
				if (result2) {
					string resString = "SUCCESS|Pay completed.\n";
					const char *res = resString.c_str();
					write(sockfd, res, strlen(res));
					cout << "Update Bill Amount successful.\n";
				} 
				else {
					string resString = "FAILURE|Pay failed.\n";
					const char *res = resString.c_str();
					write(sockfd, res, strlen(res));
					cout << "Update operation failed.\n";
				}
			}
			catch (const std::exception &e)
			{
				// If any error, send FAILURE response to the client
				string resString = "FAILURE|Pay failed.\n";
				const char *res = resString.c_str();
				write(sockfd, res, strlen(res));
				cout << "Update operation failed.\n";
			}
		}
	}
	catch (const std::exception &e)
	{
		string resString = "FAILURE|Account does not exist.\n";
		const char *res = resString.c_str();
		write(sockfd, res, strlen(res));
		cout << "Account does not exist\n";
	}
}

void handleShowClasses(const vector<string> &, int)
{
	string username = parts[1]; // Get username
	string resString = "";

	// Connect to MongoDB
	mongocxx::uri uri(MONGODB_CONNECTION_STRING);
	mongocxx::client client{uri};

	auto database = client["main"];         // Access the database
    auto collection = database["users"];   // Access the collection

	cout << "\nSuccessfully connected to MongoDB!" << endl;

	// FIlter to find the user
	bsoncxx::builder::stream::document filter_builder{};
	filter_builder << "username" << username;

	try
	{
		auto result = collection.find_one(filter_builder.view());

		if (result)
		{
			auto doc = *result;

			if (doc["classes"])		// Checks whether the user has any classes registered
			{
				auto classes_array = doc["classes"].get_array().value;
				resString = "SUCCESS|Here are your classes:\n";

				for(const auto &class_doc : classes_array)
				{
					string class_name = class_doc["name"].get_string().value.to_string();
					resString += "- " + class_name + "\n";
				}

			}
			else
			{

				resString = "SUCCESS|You have not registered for any classes.\n";

			}
		}
		else
		{
			resString = "FAILURE|USer not found.\n";
		}
	}
	catch(const std::exception& e)
	{
		// Send FAILURE response to client
		resString = "FAILURE|Could not get classes.\n";
		const char *res = resString.c_str();
		write(sockfd, res, strlen(res));
		std::cerr << "ERROR: " << e.what() << endl;
	}
	
}