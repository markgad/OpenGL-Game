# Save The Planet! - OpenGL Game

A 3D flight game built with OpenGL/GLUT for a Computer Graphics course.

## About
Fly around a dynamic wavy sphere (Earth) while avoiding red obstacles and collecting cyan power-ups. Score 20 points within 60 seconds to win!

## Controls
- **Arrow Keys**: Move the plane (Up/Down/Left/Right)
- **Mouse**: Click menu buttons

## How to Play
- Avoid red spheres (obstacles)
- Collect cyan spheres (+1 point, +2 seconds)
- Don't crash into the planet surface
- Reach 20 points to win

## Compilation
```bash
# Linux/Mac
g++ -o aviator game.cpp -lGL -lGLU -lglut

# Windows
g++ -o aviator.exe game.cpp -lopengl32 -lglu32 -lfreeglut
```

## Requirements
- OpenGL
- GLUT/FreeGLUT
- C++ compiler

---
**Course Project**: Computer Graphics  

