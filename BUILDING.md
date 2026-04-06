# Building OpenXcom with CMake on Windows

This document describes the steps to build OpenXcom using CMake on Windows with Visual Studio Build Tools.

## Prerequisites

- Windows 10/11
- Git
- CMake 4.1.0 or later
- Visual Studio Build Tools 2017 or later (we used Visual Studio 2022 Build Tools)

## Step-by-Step Build Instructions

### 1. Install CMake

If CMake is not already installed, install it using winget:

```powershell
winget install Kitware.CMake
```

After installation, refresh your PATH environment variable:

```powershell
$env:PATH = [System.Environment]::GetEnvironmentVariable("PATH","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("PATH","User")
```

Verify CMake installation:

```powershell
cmake --version
```

### 2. Download Dependencies

OpenXcom requires SDL 1.2 libraries. Download the pre-compiled dependency package:

```powershell
Invoke-WebRequest -Uri "http://openxcom.org/download/dev/openxcom-deps-win-vc2017.zip" -OutFile "openxcom-deps-win-vc2017.zip"
```

### 3. Extract Dependencies

Extract the dependencies to the project root directory:

```powershell
Expand-Archive -Path "openxcom-deps-win-vc2017.zip" -DestinationPath "." -Force
Remove-Item "openxcom-deps-win-vc2017.zip"
```

This will create a `deps/` directory containing:
- `deps/include/` - Header files for SDL 1.2 and yaml-cpp
- `deps/lib/Win32/` - 32-bit libraries and DLLs
- `deps/lib/x64/` - 64-bit libraries and DLLs

### 4. Create Build Directory

Create a separate build directory to keep the source tree clean:

```powershell
mkdir build
```

### 5. Generate Build Files

Navigate to the build directory and run CMake to generate the build files:

```powershell
cd build
cmake ..
```

CMake will automatically detect the dependencies in the `deps/` directory and configure the project.

### 6. Build the Project

Build the project using CMake in Release configuration:

```powershell
cmake --build . --config Release
```

This will compile all source files and create the executable.

### 7. Deploy Runtime Dependencies

Copy the required DLLs to the executable directory:

```powershell
cd ..
Copy-Item "deps/lib/x64/*.dll" "build/bin/Release/"
```

## Build Output

After successful compilation, you will find:

- **Executable**: `build/bin/Release/openxcom.exe`
- **Required DLLs**: All SDL 1.2 and dependency DLLs in the same directory
- **Build files**: Visual Studio project files in `build/` directory

## Dependencies Included

The dependency package includes:

- SDL 1.2.15 (SDL.dll)
- SDL_mixer 1.2.12 (SDL_mixer.dll)
- SDL_image 1.2.12 (SDL_image.dll)
- SDL_gfx 2.0.22+ (SDL_gfx.dll)
- yaml-cpp (yaml-cpp.dll)
- Supporting libraries: FLAC, JPEG, PNG, TIFF, Vorbis, WebP, zlib

## Running OpenXcom

Before running OpenXcom, you need to copy the X-COM game data files as described in the main README.md file.

The executable can be found at: `build/bin/Release/openxcom.exe`

## Troubleshooting

### CMake Configuration Issues

If CMake fails to find dependencies, ensure:
1. The `deps/` directory exists in the project root
2. The `deps/include/` and `deps/lib/` directories contain the required files
3. You're using the correct dependency package for your Visual Studio version

### Build Errors

If compilation fails:
1. Ensure you have Visual Studio Build Tools installed
2. Try cleaning the build directory and regenerating: `rm -rf build && mkdir build`
3. Check that all required DLLs are present in `deps/lib/x64/`

### Runtime Issues

If the executable fails to start:
1. Ensure all DLLs from `deps/lib/x64/` are copied to `build/bin/Release/`
2. Install Visual C++ Redistributable if needed
3. Copy X-COM game data files to the appropriate directory

## Alternative Build Methods

This project also supports:
- Visual Studio solution files (see `src/OpenXcom.2010.sln`)
- MinGW compilation
- Make-based builds

For other build methods, refer to the [OpenXcom wiki](https://ufopaedia.org/index.php/Compiling_(OpenXcom)).
