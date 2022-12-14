# Tokio
An ambitious project to make an easy-to-use but powerful reverse-engineering tool.

## Development
The project is still in its development phase and not yet ready to be released.

## What features will be available in Tokio?
- Memory and pointer scanning
- A powerful disassembler and decompiler
- C-Style struct parser and memory browser
- Debugger and remote debugger
- Support for x86, x86-64 and ARM
- Extendable plugins/backend for more different architectures
- Support for users-own bypass methods

## Screenshots
![](/Screenshots/DisassemblerBeta.png)
![](/Screenshots/SnowmanDecompiler.png)

## Dependencies
**Without these awesome open source projects, Tokio wouldn't have been possible! ❤**
- [ImGui](https://github.com/ocornut/imgui): Dear ImGui is a bloat-free graphical user interface library for C++. [(MIT License)](https://github.com/ocornut/imgui/blob/master/LICENSE.txt)
- [Zydis](https://github.com/zyantific/zydis): Fast and lightweight x86/x86-64 disassembler and code generation library. [(MIT License)](https://github.com/zyantific/zydis/blob/master/LICENSE)
- [yegord's fork of snowman](https://github.com/yegord/snowman): Snowman is a native code to C/C++ decompiler, supporting x86, AMD64, and ARM architectures. (No license)

And a massive thanks to:
- [ocornut](https://github.com/ocornut): The author of the ImGui library, the silence hero!
- [bitwizeshift](https://github.com/bitwizeshift/result): The author of the Result library, which Tokio used at the beginning, unfortunately it is now switched to the good old exceptions.
- All users, followers and contributors of Tokio, I love you all 💖.

## Build instructions
The project is made to build with Visual Studio 2022.
<br>
To build it, you first have to install TrueType as it is used for ImGui. Follow ImGui's instructions [here](https://github.com/ocornut/imgui/blob/master/docs/FONTS.md#using-freetype-rasterizer-imgui_freetype), or run the commands below to install it from vckpkg:
```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
./vcpkg integrate install
./vcpkg install freetype:x64-windows
```
For the snowman decompiler, Qt5 and boost has to be installed as well:
```
./vcpkg install boost:x64-windows
./vcpkg install qt5:x64-windows
```

## Going Cross-platform
Although Tokio was originally developed for Windows, it has the potential of going cross-platform since the core engine is os-independent, there are a few changes required to do it such as replacing Graphics.cpp, make Win32Memory and Win32Symbol engine a plugin, re-design the plugin interface to load other than dlls, and so on.
