CFLAGS = -g -Wall -std=c++11

all: db-verify db-dump protobuf-parser

gemstone.pb.h:
	protoc --cpp_out=./ gemstone.proto

gemstone-id.o: gemstone-id.cc gemstone-id.h
	g++ $(CFLAGS) -c gemstone-id.cc

gemstone.pb.o: gemstone.pb.h
	g++ $(CFLAGS) -c gemstone.pb.cc

db-verify.o: db-verify.cc constants.h
	g++ $(CFLAGS) -c db-verify.cc

db-dump.o: db-dump.cc constants.h
	g++ $(CFLAGS) -c db-dump.cc

db-verify: gemstone.pb.o db-verify.o gemstone-id.o
	g++ gemstone.pb.o gemstone-id.o db-verify.o -lleveldb `pkg-config --libs protobuf` -o $@

db-dump: gemstone.pb.o db-dump.o
	g++ gemstone.pb.o db-dump.o -lleveldb `pkg-config --libs protobuf` -o $@

protobuf-parser.o: protobuf-parser.cc constants.h
	g++ $(CFLAGS) -c protobuf-parser.cc `pkg-config --cflags protobuf`

protobuf-parser: gemstone.pb.o protobuf-parser.o
	g++ gemstone.pb.o protobuf-parser.o `pkg-config --libs protobuf` -o $@

clean:
	rm -f *.o
	rm -f gemstone.pb.*
	rm -f db-verify db-dump protobuf-parser

