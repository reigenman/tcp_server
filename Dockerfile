FROM ubuntu:20.04

ENV TZ=Europe/Moscow
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y python3 python3-pip cmake git \
    && pip3 install conan && conan profile detect


RUN mkdir -p /workspace/tcp_server

COPY . /workspace/tcp_server

WORKDIR /workspace/tcp_server

RUN conan install . -of=./build --settings=build_type=Release --build=missing --settings=compiler.cppstd=17 \
    && cmake --version

RUN cd build && cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=./build/Release/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release \
    && cmake --build . \
    && ctest -V

RUN cd build && cmake --install apps --prefix /workspace

ENTRYPOINT [ "/workspace/bin/simple-server" ] 