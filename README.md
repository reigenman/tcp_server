# tcp_server

Simple tcp async server using boost::asio in callback style.

Returns SHA256 using openssl.
How to run:
```
Allowed options:
  --help                produce help message
  --addr arg (=0.0.0.0) tcp address to listen
  --port arg (=8088)    tcp port to listen

# examples
simple-server

simple-server --port 9090
simple-server --addr 127.0.0.1
simple-server --addr 0.0.0.0 --port 8090
```

# Timeout for an idle connection
If the connection is idle for 60 seconds the server will close it.

# install conan and confgure default profile

```bash
pip install --user conan

conan profile detect
```

# install dependencies with debug options
Run this command in the source tree

```bash
conan install . -of=./build --settings=build_type=Release --build=missing --settings=compiler.cppstd=17
```

# for old cmake-3.16 on ubuntu 20.04 
```bash
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=./build/Release/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release
cmake --build .
# to run tests
ctest -V
```

# if using CMake>=3.23
```bash
cmake --preset conan-release
cmake  --build  --preset conan-release
```

# Dockerfile

To simple build and run the app there is Dockerfile.

```
docker build . -t test/tcp-server
docker run --rm -it -p 8088:80 test/tcp-server  --addr 0.0.0.0 --port 80 
```

