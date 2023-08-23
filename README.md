# 2Q cache algorithm

`2Q algorithm` solves LRU caching problems by introducing parallel buffers and supporting queues. Instead of considering just recency as a factor, 2Q also considers access frequency while making the decision to ensure the page that is really warm gets a place in the LRU cache. It admits only hot pages to the main buffer and tests every page for a second reference.
![algorithm](https://github.com/Agustangel/2Q-cache-algorithm/blob/master/visual%20algorithm.png)
## 1. How to build

### Linux
```sh
makedir build-debug
cmake -B build-debug/ -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug/
# Build in release preset
cd build/
make
```

## 2. How to run tests
```sh
# Navigate to build directory and run
cd build/
ctest
```
