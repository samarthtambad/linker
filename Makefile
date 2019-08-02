linker: linker.cpp
	g++ -std=c++14 linker.cpp -o linker
clean:
	rm -f linker *~