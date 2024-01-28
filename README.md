# tcp_server

# install conan and confgure default profile
```bash
pip install --user conan

conan profile detect
```

# install dependencies

```bash
conan install conanfile.txt -of=build --settings=build_type=Debug --build=missing
```