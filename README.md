# Run instructions
```bash
source ./setup_env.sh

mkdir build
cd build

# For debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug && make

# For release build
cmake .. -DCMAKE_BUILD_TYPE=Release && make
```
