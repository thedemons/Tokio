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

## Dependencies
**Without these awesome open source project, Tokio wouldn't have been possible! ❤**
- [ImGui](https://github.com/ocornut/imgui): Dear ImGui is a bloat-free graphical user interface library for C++. [(MIT License)](https://github.com/ocornut/imgui/blob/master/LICENSE.txt)
- [Zydis](https://github.com/zyantific/zydis): Fast and lightweight x86/x86-64 disassembler and code generation library. [(MIT License)](https://github.com/zyantific/zydis/blob/master/LICENSE)
- [Result](https://github.com/bitwizeshift/result): A modern, simple, and light-weight error-handling alternative to exceptions with a rich feature-set. [(MIT License)](https://github.com/bitwizeshift/result/blob/master/LICENSE)

## Build instructions
The project is made to build with Visual Studio 2022.
<br>
To build it, you first have to install TrueType as it is used for ImGui. Follow ImGui's instructions [here](https://github.com/ocornut/imgui/blob/master/docs/FONTS.md#using-freetype-rasterizer-imgui_freetype), or run the commands below to install it from vckpkg:
```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install freetype --triplet=x64-windows
```
