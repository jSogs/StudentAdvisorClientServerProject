#include "unp.h"
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>

void connect_db(){
    mongocxx::instance instance{};
    try {
        // Custom connection URI
        mongocxx::uri uri("mongodb+srv://julianasogwa96:o5zKPuqoOnAup7PJ@cluster0.kl71a.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0");
        mongocxx::client client{uri}; // Connect to MongoDB
        
        auto database = client["main"];          // Access the database
        auto collection = database["users"];    // Access a collection

        std::cout << "Successfully connected to MongoDB!" << std::endl;

        // Perform operations like find or insert (example)
        auto cursor = collection.find({});
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
    }
}

int main(int argc, char**argv){
    connect_db();
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(3021);

    listen(listenfd, LISTENQ);

    for(;;){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *) &cliaddr, &clilen);

        if((childpid = fork()) == 0){
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
        wait(NULL);
    }
}