# AssaultCube Cheat - Improved Edition

An enhanced aimbot for AssaultCube game on Linux with improved performance, reliability, and features.

## Features

### Core Functionality
- **Aimbot**: Automatically aims at enemies when right-clicking
- **Head targeting**: Targets enemy heads for maximum damage
- **Distance-based targeting**: Prioritizes closest enemies
- **Live entity detection**: Filters out dead enemies

### Improvements Made
- ✅ **Fixed compilation errors**: Removed duplicate code, fixed syntax errors
- ✅ **Automatic mouse detection**: No need to hardcode input device paths
- ✅ **20x faster responsiveness**: Reduced update time from 1000ms to 50ms
- ✅ **Better targeting logic**: Distance-based priority system
- ✅ **Enhanced error handling**: Graceful failure when game/devices not found
- ✅ **Configuration system**: Easily adjustable parameters
- ✅ **User-friendly interface**: Help system and status messages

## Building

```bash
make clean
make all
```

## Usage

### Main Aimbot
```bash
./cheat           # Run the aimbot
./cheat --help    # Show help and configuration
./cheat --version # Show version info
```

### Debug Tool
```bash
./main            # Show game memory information
```

## Controls

- **Right-click**: Activate aimbot on the enemy you're looking at
- **Ctrl+C**: Exit the program

## Configuration

The cheat includes configurable parameters:

- **Head offset**: 1.0 units (Z-axis offset for headshots)
- **Max target distance**: 100.0 units (maximum targeting range)
- **Update frequency**: 50ms (how often to check for targets)
- **View tolerance**: 5 degrees (how precisely you need to aim)

## Technical Details

### Architecture
- **cheat.c**: Main aimbot with improved targeting logic
- **main.c**: Debug tool for memory analysis
- **memoryManagement.c**: Game memory reading/writing functions
- **mathTool.c**: 3D vector math and targeting calculations
- **offsets.h**: Game memory offsets for AssaultCube

### Key Improvements
1. **Modular design**: Separated concerns into proper modules
2. **Robust input**: Automatic mouse device detection
3. **Smart targeting**: Distance-based enemy prioritization
4. **Better performance**: 20x faster update cycle
5. **Error handling**: Graceful failure modes
6. **User experience**: Help system and clear feedback

## Requirements

- Linux system
- AssaultCube game running
- Root privileges (for memory access)
- Input device access permissions

## Legal Notice

This tool is for educational purposes only. Use responsibly and in accordance with game terms of service and local laws.