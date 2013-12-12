Tools for Woven database and protobuf debugging

BUILDING

Copy the gemstone.proto file from gemstone-server's repository to this directory.
You'll need protobuf and leveldb libraries, which are easy to get on osx with brew:

$ brew install protobuf
$ brew install leveldb

TOOLS

* db-verify
Prints information about the content of the database and verifies consistency.

* db-dump
Prints all data entries (so no indices).