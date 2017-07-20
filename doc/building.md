# Building Papageno

## Cross-Build for AVR-GCC

The following assumes that you have a working cross-build environment for
avr-gcc.

```
# Change current directory to the root of the Papageno git repository
#
cd <papageno_git_root>

# Create and switch to a build directory (always prefer out-of-source builds)
#
mkdir -p build/avr-gcc
cd build/avr-gcc

# Configure for cross-plaform
#
cmake -DCMAKE_TOOLCHAIN_FILE=$PWD/../../cmake/toolchains/Toolchain-avr-gcc.cmake ../..

# Run the build
#
make
```

This will create a library `libpapageno.a` in `<papageno_git_root>/build/avr-gcc/src`.