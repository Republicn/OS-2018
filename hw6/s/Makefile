all: clean server utils client
	
server: utils
	g++ -std=c++11  -Wall -Wno-pointer-arith server.cpp -o server
	
client: utils
	g++ -std=c++11  -Wall -Wno-pointer-arith client.cpp -o client
	
utils: 
	g++ -std=c++11  -Wall -Wno-pointer-arith utils.h -o utils
	
clean:
	rm -f server client utils
