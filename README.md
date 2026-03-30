# How to build.

## Get dependencies and assets:

**Manual setup**  
### SDL
Download SDL3 sources
- https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.4.2.zip
```bash
mkdir dependencies
```
unpack SDL3 and rename to `SDL` and place into `dependencies`.  

### PhysFS
Download PhysFS sources
- https://github.com/icculus/physfs/archive/refs/tags/release-3.2.0.zip
```bash
mkdir dependencies
```
unpack PhysFS and rename to `physfs` and place into `dependencies`.  

### Models/Textures
Download from Sync&Share
- https://syncandshare.lrz.de/dl/fiXxtfc7YgH8Dg6u2paF4G/.dir
Make new directory called `assets/`.
Extract the obj files to `assets/models`.

**Automatic setup (UNIX only). curl must be installed.**  
call
```bash
./setup.sh
```
done.

## Build:
```bash
cmake -B build -S .
cmake --build build/
```

Executables will be in `build`. On Windows in `build/Debug` (and `build/Release` for a release build).


## How to load assets in your code.
Ramen uses PhysFS which provides a virtual filesystem. That is, you **mount**
a system directory which will become your **root**-directory. You then
load all files relative to that **root** directory. Note that you cannot
escape the root directory! All you have to do for this to work is call
```c
Filesystem* pFS = Filesystem::Init(argc, argv, "assets");
```
At the beginning of your program. This will mount the `assets/` directory
relative to your CWD as root. So if you call your program from the location:
`/home/myhomedir/computergraphics/` PhysFS expects the folder `assets` to be
at that location (`/home/myhomedir/computergraphics/assets/`). Now you can
load eg. models via:
```c
model.Load("models/Skull.obj");
```
This loads `Skull.obj` from location `assets/models`.

You can also provide the directory to mount via command-line to your program:
```bash
./myprogram /home/me/somefolder/
```
Relative paths work as well:
```bash
./myprogram somefolder/relative/to/cwd/
```
If you provide a path via command line, then the last argument of `Filesystem::Init` will
be ignored as it has lower precedence.  
If you neither provide a path via command line nor via an argument to `Filesystem::Init`
then the CWD will be mounted automatically.
