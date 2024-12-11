# StudentAdvisorClientServerProject
A TCP Client Server Project

How to connect mongodb:
- First follow the steps in this link: https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/ or do this:

- Run the following commands:
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r4.0.0/mongo-cxx-driver-r4.0.0.tar.gz

tar -xzf mongo-cxx-driver-r4.0.0.tar.gz

cd mongo-cxx-driver-r4.0.0/build

cmake ..                                \
    -DCMAKE_BUILD_TYPE=Release          \
    -DCMAKE_CXX_STANDARD=17

cmake --build .
sudo cmake --build . --target install

- After this, run make


Client-Server Program for Student Management
Overview
This is a client-server program designed to assist students in managing their academic activities. The system enables students to perform tasks such as signing up, logging in, checking grades, registering for classes, paying bills, and viewing/dropping classes. The server integrates with a MongoDB database to store and retrieve student data.

Prerequisites
MongoDB C++ Driver: Ensure you have installed the MongoDB C++ driver. Installation instructions are provided below.
Clang++ Compiler: Required for compiling the server-side application.
MongoDB Server: Ensure a MongoDB server is running locally or remotely.
Step 1: Installing MongoDB C++ Driver
To connect to MongoDB, follow the steps in the MongoDB C++ Driver Installation Guide.

Choose your operating system (Mac or Windows) and follow the installation instructions.
Complete the installation before proceeding to the next step.

File Descriptions
client.cpp
Implements the client-side logic.
Establishes a connection to the server and communicates user requests.
Executes the main menu for students to perform tasks.
mainMenu.cpp
Defines the client-side menu operations.
Handles user input for tasks like signing up, logging in, checking grades, and registering for classes.
server.cpp
Implements the server-side logic.
Listens for incoming client connections and spawns child processes to handle requests.
Communicates with MongoDB to manage user data.
serverMenu.cpp
Defines server-side operations for handling client requests.
Includes tasks like processing logins, class registrations, and billing.
unp.h
Contains utility functions and constants used across the project.
Includes socket setup, data handling, and helper functions for splitting strings and generating grades.

Step 2: Running the Program
Run the Server: Execute the compiled server program to start listening for client connections.
Run the Client: Compile and execute the client program to interact with the server.
Features
Signup and Login: Secure user authentication and account creation.
Class Registration: Students can view available classes and register for them.
Grades: Fetch and display grades for registered classes.
Billing: View and pay tuition or class fees.
Class Management: View or drop registered classes.
Troubleshooting
MongoDB Connection Issues: Ensure the connection string and MongoDB server are configured correctly.
Compilation Errors: Verify MongoDB driver installation paths and system dependencies.
Server or Client Errors: Check logs for error messages and ensure the server is running before connecting with the client.

Enjoy managing student data seamlessly with this client-server program! 🎓
