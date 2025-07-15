[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![MIT][license-shield]][license-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
<h3 align="center">BruhOS</h3>

  <p align="center">
    Operating System Bootloader & Kernel
    <br />
  </p>
</div>


<!-- ABOUT THE PROJECT -->
## About
This project was originally done for a program in the [Davidson Institute Of Science Education](https://davidson.weizmann.ac.il/en)  
during my final year of High School (Grade 12)  
My target platform is x86_64 as it is by far the most common ISA currently  
Do note that, while the final goal was (obviously...) a little too ambitious, most of my aims were met:    
* Modern UEFI Bootloader, Using GNU-EFI & OVMF, written in C & inline assembly
* A Simple OS kernel, featuring:
    - Simple Font & Text Rendering, enabled by [UEFI-GraphicsOutputProtocol](https://wiki.osdev.org/GOP)
    - IA-32e Virtual Memory Support at the [Higher Half](https://wiki.osdev.org/Higher_Half_Kernel) - Kernel resides in the last 512GiB of the Virtual Address Space
    - Basic Utility Libraries, including but not limited to [prink](https://github.com/inonitz/bruhOS/blob/main/projects/kernel/include/std/io.h)
    - Global Descriptor Table (Kind of required but i'll mention it anyways ;) 
    - An Interrupt Descriptor Table
        * I have an abstraction to dynamically manage interrupts, see [router.h](https://github.com/inonitz/bruhOS/blob/main/projects/kernel/include/interrupts/router.h)
    - Basic ACPI Support, specifically with the I/O APIC & the Local APIC
    - Kernel Timers using HPET & Local APIC Drivers
    - A Physical Memory Manager using a Page-Granular (4096 Byte) Buddy Allocator Array
    - A Virtual Memory Manager using AVL Trees (Honestly this part is untested, and needs further improvements during scheduling features)
    - Atomics mini-library using GCC & clang Specific compiler extensions
    - [SMP](https://wiki.osdev.org/Symmetric_Multiprocessing) support, Initialization of all available cores and Core-specific data structures, each cpu needs its own:
        * Task State Segment
        * Stack
        * Many more scheduling structures... (That I've yet to implement & test :) )

#### Currently the up-to-date branch is [master](https://github.com/inonitz/bruhOS/tree/master)
**[NOTE]:** I'll eventually migrate 'master' to an up-to-date 'dev' branch and a legacy 'master', containing the unmodified code from ~4 Years ago when I initially finished the project 


<!-- ABOUT THE PROJECT -->
### Considerations
The original motivation was genuine curiousity for lower-level mechanisms & abstractions during my first years of programming -   
wanting to know how everything worked under the hood  
I also found writing software to interact with hardware incredibly interesting


### Project Structure
This project follows my [premake5-workspace-template](github.com/inonitz/premake5-workspace-template) Template repository  
Originally I only used a makefile, but I've graduated from that since then :)   
- **[NOTE]:** This project is a heavily modified version of the 'barebones' branch


### Built With / Technologies Used
<br></br>
<br>
[<img height="150px" src="https://avatars.githubusercontent.com/u/11135954?s=150&v=4">][Premake-url]
[<img height="150px" src="https://avatars.githubusercontent.com/u/2137033?s=150&v=4">][Qemu-url]
</br>

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites
* [premake5](https://premake.github.io/docs/)    (Generating makefiles)
* [qemu](https://github.com/qemu/qemu)
* [mtools](https://www.gnu.org/software/mtools/) (Crafting an ISO)
* [GDB](https://sourceware.org/gdb/)/[LLDB](https://lldb.llvm.org/use/map.html) (I use lldb in this project)
* [GCC Cross Compiler](https://github.com/lordmilko/i686-elf-tools/releases/tag/13.2.0)/Clang With LLD (for lld-link)  
- #### Take extra caution when using prebuilt compilers, as they will automatically   include system headers & libraries to the compilation commands, even on freestanding targets
* #### On Linux:
  Just get the tools required, there should not be many/if any problems

* #### On Windows:
  Personally I've had luck only with [ELF-Cross-Tools](https://github.com/lordmilko/i686-elf-tools) & [Msys2](https://www.msys2.org), specifically the following packages:
  - Msys2-Mingw64 Terminal (Only this Environment can supply mtools & clang for all I know)
  - [mtools](https://packages.msys2.org/packages/mingw-w64-x86_64-mtools)
  - [clang](https://packages.msys2.org/packages/mingw-w64-x86_64-clang)
  - [lld](https://packages.msys2.org/packages/mingw-w64-x86_64-lld)
  - [lldb](https://packages.msys2.org/base/mingw-w64-lldb)
  - [clangd & friends](https://packages.msys2.org/packages/mingw-w64-x86_64-clang-tools-extra)
  - **Do Note that ELF-Cross-Tools is used for the kernel, and msys2-mingw64-clang is used for the bootloader (I have not found a better setup yet...)**


### Installation
```sh
git clone -b master https://github.com/inonitz/bruhOS.git
# In the future
git clone -b legacy https://github.com/inonitz/bruhOS.git
git clone -b dev https://github.com/inonitz/bruhOS.git
# Don't forget to add your own remote repo
git remote set-url origin your_github_username/your_fancy_os_kernel_name
git remote -v
```

<!-- USAGE EXAMPLES -->
## Usage

call ```premake5 --help``` in the root of the repository

#### Common Commands:
```sh
    premake5 cleanproj --proj=kernel 
    premake5 cleanbuild
    premake5 cleancfg
    premake5 cleanclangd
    premake5 cleanall 
    premake5 export-compile-commands
    premake5 ecc (same as export-compile-commands)
```
#### The following commands build the whole project:
```sh
premake5 ecc && premake5 gmake
make config=Debug_amd64 -j 16 kernelimage
```
#### The following 3 targets are defined in the makefiles:
  * bootloader
  * kernel
  * kernelimage - Packages the kernel & bootloader together to an ISO image
#### There are 3 configurations available:
  * Debug_amd64
  * Release_amd64
  * RelWithDebInfo_amd64
  * **You can add more Platforms/Configurations in the `workspace.lua` file** (I only support AMD64)


<!-- ROADMAP -->
## Roadmap
- Utilizing [Profile-Guided Optimization](https://clang.llvm.org/docs/UsersManual.html#profile-guided-optimization)
- Automating setup with Docker
- Bringing back SMP Support (Relocation Errors)
- Syscalls
- Context Switching And Per-process Control Blocks
- Task Scheduling (MLFQ)
- ext2 file system
- USB 3.0 Support
  * Specifically USB Keyboard & Mouse Support is more relevant
- PCIe Driver Support
- Userspace (?)
- Profit (????)


<!-- CONTRIBUTING -->
## Contributing
I honestly Don't Care. open an issue if you're interested, I'll be sure to give it my attention

<!-- LICENSE -->
## License
Distributed under the MIT License. See `LICENSE` file.


<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements
* [PonchoOS](https://github.com/Absurdponcho/PonchoOS) For a great initial youtube guide & base skeleton for my project
* [Jarod42](https://github.com/Jarod42/premake-export-compile-commands/tree/Improvements) For the Improvements branch of export-compile-commands
* [Best-README](https://github.com/othneildrew/Best-README-Template)

<!-- Resources -->
## Resources / Bibilography
#### TODO Add Relevant Bookmarks for each sub-topic


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/inonitz/premake5-workspace-template?style=for-the-badge&color=blue
[contributors-url]: https://github.com/inonitz/premake5-workspace-template/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/inonitz/premake5-workspace-template?style=for-the-badge&color=blue
[forks-url]: https://github.com/inonitz/premake5-workspace-template/network/members
[stars-shield]: https://img.shields.io/github/stars/inonitz/premake5-workspace-template?style=for-the-badge&color=blue
[stars-url]: https://github.com/inonitz/premake5-workspace-template/stargazers
[issues-shield]: https://img.shields.io/github/issues/inonitz/premake5-workspace-template.svg?style=for-the-badge
[issues-url]: https://github.com/inonitz/premake5-workspace-template/issues
[license-shield]: https://img.shields.io/github/license/inonitz/premake5-workspace-template?style=for-the-badge
[license-url]: https://github.com/inonitz/premake5-workspace-template/blob/master/LICENSE
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/linkedin_username
[product-screenshot]: images/screenshot.png
[Next.js]: https://img.shields.io/badge/next.js-000000?style=for-the-badge&logo=nextdotjs&logoColor=white


[Premake-url]: https://github.com/premake/premake-core
[Premake.js]: https://avatars.githubusercontent.com/u/11135954?s=150&v=4
[Qemu-url]: https://github.com/qemu/qemu
[Qemu.js]: https://avatars.githubusercontent.com/u/2137033?s=150&v=4

