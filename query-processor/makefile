INCL = -I /usr/local/include/chilkat -I include
LIB = -L /usr/local/lib -lchilkat-9.5.0 -lresolv -lpthread -pthread

all:  build_util build_qproc build_main link

run: 
	rm -rf output/* saida.txt
	./main.out ./queries.txt ../indexer/output/merged.idx ../indexer/output/vocab.txt ../indexer/output/urls.txt 5 >> saida.txt

run1kk: 
	rm -rf output/* 1kk.txt
	./main.out ./queries.txt ../indexer/1kk/merged.idx ../indexer/1kk/vocab.txt ../indexer/1kk/urls.txt 5 >> 1kk.txt

run50k: 
	rm -rf output/* 50k.txt
	./main.out ./queries.txt ../indexer/output/merged.idx ../indexer/output/vocab.txt ../indexer/output/urls.txt 5 >> 50k.txt


test:
	rm -rf output/* saida.txt
	./main.out ./testset/queries.txt ./testset/index.idx ./testset/vocab.txt ../indexer/output/urls.txt 5 >> saida.txt

link:
	g++ -o main.out *.o $(INCL) $(LIB)

build_main:
	g++ -c main.cpp $(INCL) $(LIB)

build_qproc:
	g++ -c ./qproc/*.cpp $(INCL) $(LIB)

build_util:
	g++ -c ../util/*.cpp $(INCL) $(LIB)

clean:
	rm -rf *.o *.out *.exe output/*

reset:
	rm -rf output/* 
