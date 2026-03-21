# Building and Running C/C++ Programs on OpenWrt (MT7628AN / MIPS)

By following these instructions, you will be able to compile a program written in C/C++ and run it on the target MT7628 device.
All of the examples provided have been tested on a router **TP-Link TL-WR840N v6** flashed with OpenWrt 21.02. Host PC - x86, Win 11, WSL Ubuntu 22.04.3 LTS.

**Device Info**
- SoC: MediaTek [MT7628AN](https://deviwiki.com/wiki/MediaTek_MT7628)
- Memory: Flash - 4MB / RAM - 32MB
- Architecture: mipsel (little-endian)
- Target: ramips/mt76x8
- OS: [OpenWrt](https://openwrt.org/) (musl libc)

## Prepare SDK

Download the OpenWrt SDK for your target:

```bash
wget https://archive.openwrt.org/releases/21.02.0/targets/ramips/mt76x8/openwrt-sdk-21.02.0-ramips-mt76x8_gcc-8.4.0_musl.Linux-x86_64.tar.xz
tar -xf openwrt-sdk-*.tar.xz
cd openwrt-sdk-*
```

### Set up environment:
```bash
export STAGING_DIR=$(pwd)/staging_dir
export TOOLCHAIN_DIR=$(echo $STAGING_DIR/toolchain-*)
export PATH=$TOOLCHAIN_DIR/bin:$PATH
```

### Check toolchain:
```bash
mipsel-openwrt-linux-musl-gcc --version
mipsel-openwrt-linux-musl-g++ --version
```

If successful, each of these commands should return an output similar to the following:

<details>
  <summary>Output</summary>

  ```bash
  $ mipsel-openwrt-linux-musl-gcc --version
  mipsel-openwrt-linux-musl-gcc (OpenWrt GCC 8.4.0 r16279-5cc0535800) 8.4.0
  Copyright (C) 2018 Free Software Foundation, Inc.
  This is free software; see the source for copying conditions.  There is NO
  warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  ```
  
</details>

## Example C program

Create a new file `hello.c` and paste your code into it or use [simple example](src/hello.c).

### Build
```bash
mipsel-openwrt-linux-musl-gcc hello.c -o hello -Os -s
```
Used flags:
* `-Os` - optimize for size (reduce the binary file)
* `-s` - remove debug/symbol info (also to reduce the binary)

Approximate size of the resulting binary file `hello`: 3KB.

Use the following command to inspect result binary: 
```bash
file hello
readelf -d hello
```

## Example C++ program
[C++ example (without heavy iostream)](src/hello_cpp.cpp)

**NOTE:** I wasn't able to compile the file with `iostream` library usage, but that's actually a good thing since it helps reduce the size of the binary file.

### Build
```bash
mipsel-openwrt-linux-musl-g++ hello_cpp.cpp -o hello_cpp \
  -Os -s -fno-exceptions -fno-rtti \
  -static-libstdc++ -static-libgcc
```
Used flags:
* `-fno-exceptions` - reduces binary size and removes try/catch support (useful for lightweight firmware)
* `-fno-rtti` - reduces binary size and removes `dynamic_cast` and `typeid`
* `-static-libstdc++` - statically links the C++ std library into the binary, removes `libstdc++` dependency on the target device
* `-static-libgcc` - statically links the GCC runtime library, prevents runtime errors such as `_Unwind_Resume` missing

Approximate size of the resulting binary file: 3KB.

## Copy binary to target device

### Via scp
```bash
scp hello root@192.168.1.1:/root/
```
Replace `192.168.1.1` with your address.

**NOTE:** In my case, `scp` wasn't available on the router, so I used the following alternative (via SSH).

### Via SSH
```bash
cat hello | ssh root@192.168.1.1 "cat > /root/hello"
```

### Via wget
Start simple HTTP server on host device:
```bash
python3 -m http.server 8000
```
On router:
```bash
wget http://192.168.1.1:8000/hello -O /root/hello
chmod +x /root/hello
```

## Execute binary

After copying, set the permissions on router side:
```bash
chmod +x /root/hello
```
And run executable:
```bash
/root/hello
```