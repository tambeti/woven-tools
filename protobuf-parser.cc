#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "constants.h"
#include "gemstone.pb.h"

using namespace std;
using namespace google::protobuf::io;
using namespace gemstone_protobuf;

static void
parse_message(uint32_t type, unsigned char* buffer, int size) {
  if (!MessageType_IsValid(type)) {
    cerr << "Ignoring unknown message type " << type << endl;
    return;
  }

  MessageType msg_type = (MessageType) type;

  cout << "type: " << MessageType_Name(msg_type) << endl;
  cout << "size: " << size << endl;

  switch (msg_type) {
  case ENVELOPE: {
    Envelope envelope;
    envelope.ParseFromArray(buffer, size);
    cout << envelope.DebugString();
    break;
  }
  case USER: {
    User user;
    user.ParseFromArray(buffer, size);
    cout << user.DebugString();
    break;
  }
  case SOURCE: {
    Source source;
    source.ParseFromArray(buffer, size);
    cout << source.DebugString();
    break;
  }
  case GROUP: {
    Group group;
    group.ParseFromArray(buffer, size);
    cout << group.DebugString();
    break;
  }
  case PHOTO: {
    Photo photo;
    photo.ParseFromArray(buffer, size);
    cout << photo.DebugString();
    break;
  }
  }

  cout << endl;
}

static uint32_t read_uint(const unsigned char* bytes) {
  uint32_t v;
  memcpy(&v, bytes, 4);
  return __builtin_bswap32(v);
}

static void
parse_stream(istream* stream) {
  int buffer_size = 4096;
  unsigned char* bytes = new unsigned char[buffer_size];

  int offset = 0;

  while (stream->read((char *) bytes, 5)) {
    cout << "offset: " << offset << endl;

    uint32_t type = bytes[0];
    uint32_t size = read_uint(bytes + 1);
    offset += 5 + size;

    if (size > buffer_size) {
      cerr << "Really big message: " << size << " resizing" << endl;
      free(bytes);
      buffer_size *= 2;
      bytes = (unsigned char *)malloc(buffer_size);
    }

    if (stream->read((char *) bytes, size)) {
      parse_message(type, bytes, size);
    } else {
      cerr << "Unexpected EOF" << endl;
      break;
    }
  }

  delete bytes;
}

int
main(int argc, char *argv[]) {

  if (argc > 1) {
    ifstream s(argv[1], ifstream::in | ifstream::binary);
    if (!s) {
      cerr << "error reading " << argv[1] << endl;
      return 1;
    }

    parse_stream(&s);
  } else {
    parse_stream(&cin);
  }

  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
