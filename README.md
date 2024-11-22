# StudentAdvisorClientServerProject
A TCP Client Server Project

How to connect mongodb:
- First follow the steps in this link: https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/

- Choose Mac or Windows depending on your device

- After this, compile using the following command: clang++ -std=c++17 server.cpp -o output -stdlib=libc++ \
    -I/usr/local/include/mongocxx/v_noabi \
    -I/usr/local/include/bsoncxx/v_noabi \
    -I/usr/local/include/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc \
    -L/usr/local/lib -lmongocxx -lbsoncxx \
    -Wl,-rpath,/usr/local/lib
