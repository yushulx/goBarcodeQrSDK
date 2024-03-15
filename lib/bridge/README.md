## How to Build the Shared Library 

- Windows:

    ```bash
    mkdir build
    cd build
    cmake -DCMAKE_GENERATOR_PLATFORM=x64 ..
    cmake --build . --config Release
    ```
- Linux:

    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release
    ```