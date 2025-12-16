# UCommon Scripts

This directory contains utility scripts for batch processing textures.

## ConvertBinToImages

Batch convert all `.bin` files (FTex2D serialized format) to image files (PNG/HDR).

### Usage

#### PowerShell

```powershell
# Convert all bin files in bin/TexBin (default)
.\scripts\ConvertBinToImages.ps1

# Specify custom input directory
.\scripts\ConvertBinToImages.ps1 -InputDir "path\to\input"

# Specify custom output directory
.\scripts\ConvertBinToImages.ps1 -OutputDir "path\to\output"

# Force PNG output for all files (even Float types)
.\scripts\ConvertBinToImages.ps1 -ForcePng

# Combine options
.\scripts\ConvertBinToImages.ps1 -InputDir "bin\Lightmap" -OutputDir "bin\Lightmap\images" -ForcePng
```

#### Batch File (Windows)

```batch
REM Convert with default settings
scripts\ConvertBinToImages.bat

REM Pass parameters
scripts\ConvertBinToImages.bat -InputDir "bin\Lightmap" -OutputDir "output"
```

### Parameters

- **`-InputDir`** (optional): Input directory containing `.bin` files
  - Default: `bin\TexBin`
  - Example: `-InputDir "bin\Lightmap"`

- **`-OutputDir`** (optional): Output directory for converted images
  - Default: `bin\TexBin\converted`
  - Example: `-OutputDir "bin\images"`

- **`-ForcePng`** (optional): Force PNG output for all textures
  - Default: Auto-detect (Uint8 → PNG, Float/Double → HDR)
  - Example: `-ForcePng`

### Output Format

The script automatically determines the output format based on the texture's element type:

- **Uint8** → `.png` (8-bit per channel)
- **Float** → `.hdr` (32-bit floating point)
- **Double** → `.hdr` (converted to Float first)
- **Half** → `.hdr` (converted to Float first)

Use `-ForcePng` to override and convert all textures to PNG format (with clamping to 0-255 range).

### Example Output

```
========================================
Converting 21 bin files to images
========================================

[1/21] Converting: Lightmap_26DD9DC4F7D66D881A9E826FA5CD0BB1_0.bin
  ✓ Success: Lightmap_26DD9DC4F7D66D881A9E826FA5CD0BB1_0.hdr (94821 bytes)
[2/21] Converting: Lightmap_26DD9DC4F7D66D881A9E826FA5CD0BB1_1.bin
  ✓ Success: Lightmap_26DD9DC4F7D66D881A9E826FA5CD0BB1_1.hdr (175783 bytes)
...

========================================
Conversion Summary
========================================
Total files:    21
Successful:     21
Failed:         0
Output directory: D:\Workspace\UCommon\bin\TexBin\converted
```

### Requirements

- **BinToTex** application must be built first
- Run `cmake --build build --config Release --target UCommon_Apps_BinToTex` if not already built
- The script will check for the executable and report an error if not found

### Error Handling

The script includes comprehensive error handling:
- Checks if BinToTex executable exists
- Validates input directory
- Creates output directory if needed
- Reports success/failure for each file
- Provides summary statistics
- Returns exit code 0 on success, 1 if any failures occurred

### Technical Details

- Uses `UCommon_Apps_BinToTex.exe` for conversion
- Preserves original filenames (only changes extension)
- Supports all FTex2D element types (Uint8, Half, Float, Double)
- Handles large batches efficiently
- Progress indicator shows current file and total count

## Future Scripts

Additional scripts may be added here for:
- Batch texture processing (threshold, inpainting)
- Format conversion
- Texture analysis and validation
- Automated testing workflows

