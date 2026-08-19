# DMW Raylib

A small 2D driving and drifting practice game built in **C with raylib**.

The goal of the project is to create a simple environment for practicing driving, steering, and drifting mechanics in a parking-lot/city setting.

## Features

- 2D top-down driving
- Accelerating and braking/reversing
- Steering and drifting
- Drift-angle based movement
- Skidmarks while drifting
- Large explorable environment
- Camera that follows the car
- Camera boundaries that keep the player inside the world
- Custom car and environment textures
- High-DPI support
- Built entirely in C using raylib

## Controls

| Key | Action |
|---|---|
| `W` | Accelerate |
| `S` | Brake / Reverse |
| `A` | Steer left |
| `D` | Steer right |

## Building

The project uses `clang`, `pkg-config`, and raylib.

```bash
./build.sh
