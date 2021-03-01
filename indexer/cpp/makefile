INCL = -I /usr/local/include/chilkat -I include
LIB = -L /usr/local/lib -lchilkat-9.5.0 -lresolv -lpthread -pthread

all:  build_util build_index build_main link

run: 
	rm -rf ./saida.txt ./occ.csv output/*
	./main.out ./archive/collection.jl >> saida.txt
	# ./main.out ./archive/collection.jl

link:
	g++ -o main.out *.o $(INCL) $(LIB)

build_main:
	g++ -c main.cpp $(INCL) $(LIB)

build_index:
	g++ -c index/*.cpp $(INCL) $(LIB)

build_util:
	g++ -c util/*.cpp $(INCL) $(LIB)

clean:
	rm -rf *.o *.out *.exe output/*

reset:
	rm -rf output/* 
