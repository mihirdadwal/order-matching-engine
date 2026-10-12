# Setup – Windows

Your machine currently has **Git** and **VS Code**, but **no C++ compiler and no CMake**.

## 1. Install the compiler (g++) and CMake
Open **PowerShell** and run:

```powershell
winget install -e --id BrechtSanders.WinLibs.POSIX.UCRT
winget install -e --id Kitware.CMake
```

- **WinLibs** = GCC (g++) compiler for Windows
- **CMake** = build tool

Then **close and reopen** PowerShell / VS Code and check:

```powershell
g++ --version
cmake --version
```

Both should print a version number. If `g++` is not found, restart your PC (PATH refresh).

## 2. VS Code extensions
Install these from the Extensions tab:
- **C/C++** (Microsoft)
- **CMake Tools** (Microsoft)

## 3. Hello World test
Create `hello.cpp`:

```cpp
#include <iostream>

int main() {
    std::cout << "Hello, C++!\n";
    return 0;
}
```

Compile and run:

```powershell
g++ -std=c++17 hello.cpp -o hello.exe
.\hello.exe
```

You should see `Hello, C++!` — your setup is done ✅
