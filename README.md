Tools for Woven database and protobuf debugging

BUILDING

Copy the gemstone.proto file from gemstone-server's repository to this directory.
You'll need protobuf and leveldb libraries, which are easy to get on osx with brew:

$ brew install protobuf
$ brew install leveldb

After that, everything should be ready, compile with:

$ make

GETTING THE DATABASE FROM WOVEN

adb pull /sdcard/Android/data/com.litl.FireDrill/files ./

TOOLS

* db-verify <path to database directory>
Prints information about the content of the database and verifies consistency.

* db-dump <path to database directory>
Prints all data entries (so no indices).
