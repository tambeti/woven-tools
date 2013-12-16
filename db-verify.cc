#include <iostream>
#include <unordered_set>
#include <leveldb/db.h>

#include "gemstone.pb.h"
#include "constants.h"
#include "gemstone-id.h"

using namespace std;
using namespace gemstone_protobuf;

static void count_rows(leveldb::DB* db) {
  leveldb::ReadOptions options;
  options.verify_checksums = true;
  leveldb::Iterator* it = db->NewIterator(options);

  int total = 0;
  int envelopes = 0;
  int users = 0;
  int source = 0;
  int groups = 0;
  int photos = 0;
  int photo_pos = 0;
  int group_pos = 0;
  int group_photo = 0;

  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    char record_type = it->key()[0];
    switch (record_type) {
    case RECORD_DATA: {
      int data_type = it->key()[1];
      switch (data_type) {
      case ENVELOPE:
        envelopes++;
        break;
      case USER:
        users++;
        break;
      case SOURCE:
        source++;
        break;
      case GROUP:
        groups++;
        break;
      case PHOTO:
        photos++;
        break;
      }

      break;
    }
    case RECORD_PHOTO_POS:
      photo_pos++;
      break;
    case RECORD_GROUP_POS:
      group_pos++;
      break;
    case RECORD_GROUP_PHOTOS:
      group_photo++;
      break;
    default:
      break;
    }

    total++;
  }

  delete it;

  cout << "db has " << total << " rows" << endl;
  cout << "sources: " << source << " groups: " << groups
       << " photos: " << photos << " photo_pos: " << photo_pos
       << " group_pos: " << group_pos << " group_photo: " << group_photo << endl;

  if (envelopes != 1)
    cerr << "Invalid number of envelopes: " << envelopes << endl;

  if (users != 1)
    cerr << "Invalid number of users: " << users << endl;

  if (groups != group_pos)
    cerr << "Invalid group position index entries: " << group_pos
         << " (groups: " << groups << ")" << endl;

  if (photos != photo_pos)
    cerr << "Invalid photo position index entries: " << photo_pos
         << " (photos: " << photos << ")" << endl;

}

static long read_long(const char* bytes) {
  long v;
  memcpy(&v, bytes, 8);
  return __builtin_bswap64(v);
}

static void print_id(const char* id) {
  for (int i = 0; i < GEMSTONE_ID_LEN; ++i) {
    printf("%02x", id[i] & 0x00FF);
  }
}

static leveldb::Status lookup_group(leveldb::DB* db, const char* id, string* value) {
  int key_len = 2 + GEMSTONE_ID_LEN;
  char key[key_len];
  key[0] = RECORD_DATA;
  key[1] = GROUP;
  memcpy(key + 2, id, GEMSTONE_ID_LEN);
  leveldb::Slice slice(key, key_len);

  return db->Get(leveldb::ReadOptions(), slice, value);
}

static leveldb::Status lookup_group(leveldb::DB* db, GemstoneId& id) {
  int key_len = 2 + GEMSTONE_ID_LEN;
  char key[key_len];
  key[0] = RECORD_DATA;
  key[1] = GROUP;
  memcpy(key + 2, id.data().data(), GEMSTONE_ID_LEN);
  leveldb::Slice slice(key, key_len);

  string value;
  return db->Get(leveldb::ReadOptions(), slice, &value);
}

static leveldb::Status lookup_photo(leveldb::DB* db, const char* id, string* value) {
  int key_len = 2 + GEMSTONE_ID_LEN;
  char key[key_len];
  key[0] = RECORD_DATA;
  key[1] = PHOTO;
  memcpy(key + 2, id, GEMSTONE_ID_LEN);
  leveldb::Slice slice(key, key_len);

  return db->Get(leveldb::ReadOptions(), slice, value);
}

static leveldb::Status lookup_photo(leveldb::DB* db, GemstoneId& id) {
  int key_len = 2 + GEMSTONE_ID_LEN;
  char key[key_len];
  key[0] = RECORD_DATA;
  key[1] = PHOTO;
  memcpy(key + 2, id.data().data(), GEMSTONE_ID_LEN);
  leveldb::Slice slice(key, key_len);

  string value;
  return db->Get(leveldb::ReadOptions(), slice, &value);
}

static void validate_group_pos_index(leveldb::DB* db) {
  char key[] = { RECORD_GROUP_POS };
  leveldb::Slice slice(key, 1);

  int key_len = 1 + 8 + GEMSTONE_ID_LEN;

  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  for (it->Seek(slice); it->Valid() && it->key().starts_with(slice); it->Next()) {
    if (it->key().size() != key_len) {
      cerr << "Invalid group pos index key" << endl;
      continue;
    }

    const char* id = it->key().data() + 9;
    string value;
    if (!lookup_group(db, id, &value).ok()) {
      long sort_key = read_long(it->key().data() + 1);
      cout << "Group pos index has invalid entry: ";
      print_id(id);
      cout << " sort_key: " << sort_key << endl;
    }
  }

  delete it;
}

static void validate_photo_pos_index(leveldb::DB* db) {
  char key[] = { RECORD_PHOTO_POS };
  leveldb::Slice slice(key, 1);

  int key_len = 1 + 8 + GEMSTONE_ID_LEN;

  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  for (it->Seek(slice); it->Valid() && it->key().starts_with(slice); it->Next()) {
    if (it->key().size() != key_len) {
      cerr << "Invalid photo pos index key" << endl;
      continue;
    }

    const char* id = it->key().data() + 9;
    string value;
    if (!lookup_photo(db, id, &value).ok()) {
      long sort_key = read_long(it->key().data() + 1);
      cout << "Photo pos index has invalid entry: ";
      print_id(id);
      cout << " sort_key: " << sort_key << endl;
    }
  }

  delete it;
}

static void validate_group_photo_index(leveldb::DB* db) {
  char key[] = { RECORD_GROUP_PHOTOS };
  leveldb::Slice slice(key, 1);

  int key_len = 1 + 8 + 2 * GEMSTONE_ID_LEN;

  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  for (it->Seek(slice); it->Valid() && it->key().starts_with(slice); it->Next()) {
    if (it->key().size() != key_len) {
      cerr << "Invalid group photo pos index key" << endl;
      continue;
    }

    const char* group_id = it->key().data() + 1;
    const char* photo_id = it->key().data() + 21;

    string value;

    if (!lookup_group(db, group_id, &value).ok()) {
      cout << "Group photo index has invalid group reference: ";
      print_id(group_id);
      cout << " ";
      print_id(photo_id);
      cout << endl;
    }

    if (!lookup_photo(db, photo_id, &value).ok()) {
      long sort_key = read_long(it->key().data() + 1 + GEMSTONE_ID_LEN);
      cout << "Group photo index has invalid photo reference: ";
      print_id(group_id);
      cout << " sort_key: " << sort_key << " ";
      print_id(photo_id);
      cout << endl;
    }
  }

  delete it;
}

static void load_sources(leveldb::DB* db, unordered_set<string>& set) {
  char key[] = { RECORD_DATA, SOURCE };
  leveldb::Slice slice(key, 2);

  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  for (it->Seek(slice); it->Valid() && it->key().starts_with(slice); it->Next()) {
    GemstoneId id(it->key().data() + 2);
    set.insert(id.ToString());
  }

  delete it;
}

static void print_target_id_error(const char* msg, GemstoneId source_id,
                                  char target_type, GemstoneId target_id) {

  string target_type_str;
  switch (target_type) {
  case SOURCE_IDX_GROUP:
    target_type_str = string("group");
    break;
  case SOURCE_IDX_PHOTO:
    target_type_str = string("photo");
    break;
  case SOURCE_IDX_VIDEO:
    target_type_str = string("video");
    break;
  default:
    target_type_str = string("unknown");
  }

  cerr << "Source index error: "
       << msg << " " << source_id.ToString() << " "
       << target_type_str << " " << target_id.ToString() << endl;
}

static void validate_source_index(leveldb::DB* db) {
  unordered_set<string> sources;
  load_sources(db, sources);

  char key[] = { RECORD_SOURCE_IDX };
  leveldb::Slice slice(key, 1);

  int key_len = 1 + 1 + 8 + 2 * GEMSTONE_ID_LEN;

  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  for (it->Seek(slice); it->Valid() && it->key().starts_with(slice); it->Next()) {
    if (it->key().size() != key_len) {
      cerr << "Invalid source index key" << endl;
      continue;
    }

    GemstoneId source_id(it->key().data() + 1);
    char target_type = it->key().data()[1 + GEMSTONE_ID_LEN + 8];
    GemstoneId target_id(it->key().data() + 1 + GEMSTONE_ID_LEN + 8 + 1);

    if (sources.find(source_id.ToString()) == sources.end()) {
        print_target_id_error("source not found", source_id, target_type,
                              target_id);
    }

    switch (target_type) {
    case SOURCE_IDX_GROUP:
      if (!lookup_group(db, target_id).ok()) {
        print_target_id_error("group not found", source_id, target_type,
                              target_id);
      }
      break;
    case SOURCE_IDX_PHOTO:
    case SOURCE_IDX_VIDEO:
      if (!lookup_photo(db, target_id).ok()) {
        print_target_id_error("photo not found", source_id, target_type,
                              target_id);
      }
      break;
    default:
      break;
    }
  }

  delete it;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    string name = string(argv[0]);
    cerr << "usage: " << name << " <path to leveldb directory>" << endl;
    return 1;
  }

  string path = string(argv[1]);

  leveldb::DB* db;
  leveldb::Options options;
  options.paranoid_checks = true;

  leveldb::Status status = leveldb::DB::Open(options, path.c_str(), &db);
  if (!status.ok()) {
    cerr << "Can not open database from " << path << status.ToString() << endl;
    return 1;
  }

  count_rows(db);

  validate_group_pos_index(db);
  validate_photo_pos_index(db);
  validate_group_photo_index(db);
  validate_source_index(db);

  delete db;

  return 0;
}
