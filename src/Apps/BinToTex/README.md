# BinToTex

A command-line tool to convert FTex2D serialized binary files to image formats (PNG/HDR).

## Usage

```bash
UCommon_Apps_BinToTex <input.bin> <output.png/hdr> [--force-png]
```

### Arguments

- `<input.bin>`: Path to the input binary file (FTex2D serialized format)
- `<output.png/hdr>`: Path to the output image file

### Options

- `--force-png`: Convert to Uint8 and export as PNG regardless of input type

### Automatic Format Detection

The tool automatically determines the output format based on the ElementType in the binary file:

- **Uint8** → PNG format
- **Float** → HDR format
- **Double** → Converted to Float, then exported as HDR

### Examples

Export a float texture as HDR:
```bash
UCommon_Apps_BinToTex Cubemap_128.bin output.hdr
```

Force conversion to PNG (converts float to uint8):
```bash
UCommon_Apps_BinToTex Cubemap_128.bin output.png --force-png
```

## Build

The tool is automatically built as part of the UCommon project when CMake is configured.

```bash
cmake -B build -S .
cmake --build build --config Release --target UCommon_Apps_BinToTex
```

The executable will be placed in the `bin/` directory.

