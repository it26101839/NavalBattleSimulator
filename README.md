# Naval Battle Simulator

C program that simulates a naval battle between a stationary Battleship (B)
and several Escort Ships (E), based on projectile motion physics.
Developed for SE1012 - Programming Methodology assignment.

## Project Structure

- **part1A** - Basic battlefield setup and single attack simulation
- **part1B** - Battleship movement simulation (random path + gun jam feature)
- **part1C** - Percentage-based impact damage system
- **part2A** - Firing interval and closest-first attack strategy for the Battleship
- **part2B** - Continuous firing simulation for Escort Ships with individual firing intervals
- **menu** - Full simulator with a menu system (Start Simulation, Instructions, Statistics, Exit)

## How to Compile and Run

Each folder contains its own set of source files. To compile and run any part:

```bash
cd menu
gcc main.c setup.c physics.c fileio.c simulate.c strategy.c -o simulator -lm
./simulator
```

## Features

- Random generation of escort ships (type, position, velocity, angle range)
- Projectile motion based attack range and time-to-hit calculations
- Percentage-based cumulative damage system
- Closest-first attack strategy for the Battleship
- Continuous firing for Escort Ships with type-specific firing intervals
- Initial and final battlefield conditions saved to text files
- Menu-driven interface with simulation statistics viewer

## Author

it26101839
