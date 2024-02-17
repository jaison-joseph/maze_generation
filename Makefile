
CXXFLAGS = -std=c++17 -pthread -O3
LDFLAGS = -Wl,-stack_size,0x1000000

filename := genetic_3

final: ${filename}.o
	g++ $(LDFLAGS) ${filename}.o -o ${filename} 

${filename}.o: ${filename}.cpp
	g++ -c $(CXXFLAGS) ${filename}.cpp 

clean: 
	rm ${filename}.o
	rm ${filename}

run: 	${filename}
	./${filename}