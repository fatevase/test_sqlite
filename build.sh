 rm -rf build && mkdir build && cd ./build && cmake .. && make
# g++ src/* -I inc -o test_sqlite -lsqlite3