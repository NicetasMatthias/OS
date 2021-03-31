g++ -c lab9_server.cpp
g++ -c lab9_client.cpp
g++ -o lab9_server lab9_server.o -lpthread
g++ -o lab9_client lab9_client.o -lpthread
