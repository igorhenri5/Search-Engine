INCL = -I /usr/local/include/chilkat 
LIB = -L /usr/local/lib -lchilkat-9.5.0 -lresolv -lpthread -pthread

all: build_site build_dumper build_util build_crawler build_sched build_main link run

run: 
	./main.out seeds.txt

link:
	g++ -o main.out *.o $(INCL) $(LIB)

build_main:
	g++ -c main.cpp $(INCL) $(LIB)

build_dumper:
	g++ -c dumper/*.cpp $(INCL) $(LIB)

build_site:
	g++ -c site/*.cpp $(INCL) $(LIB)

build_crawler:
	g++ -c crawler/*.cpp $(INCL) $(LIB)

build_sched:
	g++ -c sched/*.cpp $(INCL) $(LIB)

build_util:
	g++ -c util/*.cpp $(INCL) $(LIB)

example: 
	./a.out seeds.txt $(INCL) $(LIB)

clean:
	rm -rf *.o *.out *.exe *.json *.xml *.html dump/*.out dump/pages/*.html

snd:
	g++ -o snd.out snd.cpp 
	./snd.out