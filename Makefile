CFLAGS = -g -Wall -std=c++11

all: db-verify db-dump

gemstone.pb.h:
	protoc --cpp_out=./ gemstone.proto

gemstone.pb.o: gemstone.pb.h
	g++ $(CFLAGS) -c gemstone.pb.cc

db-verify.o: db-verify.cc constants.h
	g++ $(CFLAGS) -c db-verify.cc

db-dump.o: db-dump.cc constants.h
	g++ $(CFLAGS) -c db-dump.cc

db-verify: gemstone.pb.o db-verify.o
	g++ gemstone.pb.o db-verify.o -lleveldb `pkg-config --libs protobuf` -o $@

db-dump: gemstone.pb.o db-dump.o
	g++ gemstone.pb.o db-dump.o -lleveldb `pkg-config --libs protobuf` -o $@

clean:
	rm -f *.o
	rm -f gemstone.pb.*
	rm -f db-verify db-dump

