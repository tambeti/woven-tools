CFLAGS = -g -Wall -std=c++11

all: dbreader db-to-protobuf

gemstone.pb.h:
	protoc --cpp_out=./ gemstone.proto

gemstone.pb.o: gemstone.pb.h
	g++ $(CFLAGS) -c gemstone.pb.cc

dbreader.o: dbreader.cc constants.h
	g++ $(CFLAGS) -c dbreader.cc

db-to-protobuf.o: db-to-protobuf.cc constants.h
	g++ $(CFLAGS) -c db-to-protobuf.cc

dbreader: gemstone.pb.o dbreader.o
	g++ gemstone.pb.o dbreader.o -lleveldb `pkg-config --libs protobuf` -o $@

db-to-protobuf: gemstone.pb.o db-to-protobuf.o
	g++ gemstone.pb.o db-to-protobuf.o -lleveldb `pkg-config --libs protobuf` -o $@

clean:
	rm -f *.o
	rm -f gemstone.pb.*
	rm -f dbreader db-to-protobuf

