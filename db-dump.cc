#include <iostream>
#include <leveldb/db.h>

#include "gemstone.pb.h"
#include "constants.h"

using namespace std;
using namespace gemstone_protobuf;

static void print_protobuf(leveldb::DB* db) {
  char key[] = { RECORD_DATA };
  leveldb::Slice slice(key, 1);

  gemstone_protobuf::Envelope envelope;
  gemstone_protobuf::User user;
  gemstone_protobuf::Source source;
  gemstone_protobuf::Group group;
  gemstone_protobuf::Photo photo;

  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  for (it->Seek(slice); it->Valid() && it->key().starts_with(slice); it->Next()) {
    int data_type = it->key()[1];
    string value = it->value().ToString();

    switch (data_type) {
    case gemstone_protobuf::ENVELOPE:
      if (envelope.ParseFromString(value)) {
        cout << "ENVELOPE" << endl;
        envelope.PrintDebugString();
        cout << endl;
      } else {
        cerr << "Could not parse envelope" << endl;
      }
      break;
    case gemstone_protobuf::USER:
      if (user.ParseFromString(value)) {
        cout << "USER" << endl;
        user.PrintDebugString();
        cout << endl;
      } else {
        cerr << "Could not parse user" << endl;
      }
      break;
    case gemstone_protobuf::SOURCE:
      if (source.ParseFromString(value)) {
        cout << "SOURCE" << endl;
        source.PrintDebugString();
        cout << endl;
      } else {
        cerr << "Could not parse source" << endl;
      }
      break;
    case gemstone_protobuf::GROUP:
      if (group.ParseFromString(value)) {
        cout << "GROUP" << endl;
        group.PrintDebugString();
        cout << endl;
      } else {
        cerr << "Could not parse group" << endl;
      }
      break;
    case gemstone_protobuf::PHOTO:
      if (photo.ParseFromString(value)) {
        cout << "PHOTO" << endl;
        photo.PrintDebugString();
        cout << endl;
      } else {
        cerr << "Could not parse photo" << endl;
      }
      break;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    string name = string(argv[0]);
    cerr << "usage: " << name << " <path to leveldb directory>" << endl;
    return 1;
  }

  string path = string(argv[1]);

  leveldb::DB* db;
  leveldb::Status status = leveldb::DB::Open(leveldb::Options(), path.c_str(), &db);
  if (!status.ok()) {
    cerr << "Can not open database from " << path << status.ToString() << endl;
    return 1;
  }

  print_protobuf(db);

  delete db;

  return 0;
}
