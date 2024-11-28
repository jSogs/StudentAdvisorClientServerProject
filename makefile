ALL: client server
client: client.cpp mainMenu.cpp
	clang++ -std=c++17 client.cpp mainMenu.cpp -o client -stdlib=libc++
server: server.cpp serverMenu.cpp
	clang++ -std=c++17 server.cpp serverMenu.cpp -o server -stdlib=libc++ \
    -I/usr/local/include/mongocxx/v_noabi \
    -I/usr/local/include/bsoncxx/v_noabi \
    -I/usr/local/include/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc \
    -L/usr/local/lib -lmongocxx -lbsoncxx \
    -Wl,-rpath,/usr/local/lib
clean:
	rm -r client server
