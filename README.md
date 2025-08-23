# sasrt-deluxe
Helpful DLL to fix bugs in Sonic & All-Stars Racing Transformed for PC
# Compiling
Currently, the DLL will only build under either Linux or MinGW on Windows. For Linux, you will need a MinGW compiler.
Simply clone this repo, `cd` to it, and run `make`.

```bash
git clone https://github.com/kernaltrap8/sasrt-deluxe
cd sasrt-deluxe
make -j$(nproc)
```

On Linux, it should automatically copy to your games install. It it doesn't, manually copy `out/d3d9.dll` to the games install directory (has to be next to `ASN_App_PcDx9_Final.exe`)
For Wine/Proton, you will also need to configure a environment variable for the DLL to load:

```bash
WINEDLLOVERRIDES="d3d9=n,b"
```

# Contributing
If you would like to submit patches or new features, make a PR here.
# Issues
If you have a problem, add PROTON_LOG=1 to your Steam launch options and make an issue with `~/steam-212480.log` attached.