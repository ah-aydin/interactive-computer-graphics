# Run instructions
```bash
mkdir build
cd build

# For debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug && make && ./01-hello-world

# For release build
cmake .. -DCMAKE_BUILD_TYPE=Release && make && ./01-hello-world
```
