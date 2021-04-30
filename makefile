build:
	rm -f program.exe
	g++ -g -std=c++17 -Wall main.cpp -lstdc++fs -o program.exe

test:
	rm -f program.exe
	g++ -g -std=c++17 -Wall maincatch.cpp test01.cpp -o program.exe

testall:
	rm -f program.exe
	g++ -g -std=c++17 -Wall maincatch.cpp test*.cpp -o program.exe
	
run:
	./program.exe

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./program.exe
