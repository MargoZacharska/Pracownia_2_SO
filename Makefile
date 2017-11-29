all : program

program : main.cpp
	g++ -pthread -Wall -Wextra -Werror main.cpp -o program
	
run :
	./program

