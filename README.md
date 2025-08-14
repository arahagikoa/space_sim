# Space Simulation

A C++-based space simulation project visualizing the gravitational effects of a black hole.  
Currently, the simulation renders a **2D gravitational lensing** effect, with ongoing development for a **3D black hole simulation**.
In the future I am planning on adding more elements such as: 3D simulation, thruster simulation, ...

---

## Features
- **2D Black Hole Simulation**  
  - Realistic gravitational lensing effect.
  - Ray tracing–inspired approach to simulate how light bends around a black hole.
- **Modular C++ Codebase**  
  - Clear separation between rendering logic, physics calculations, and main program flow.
- **3D Simulation (In Progress)**  
  - Upcoming version will feature a fully navigable 3D scene around a black hole.


## Project Structure
```

black_hole/
-- black\_hole/
-- black\_hole.cpp      # Black hole physics & rendering logic
-- black\_hole.h
-- engine.cpp          # Simulation engine
-- engine.h
-- main.cpp            # Entry point of the program
-- ray.cpp             # Light ray behavior and calculations
-- ray.h
black_hole-3D/

- .gitignore
- README.md
```

---

## Requirements
- C++17 or newer
- A compiler such as `g++` or `clang++`
- OpenGL for visualization, depending on your setup

---

## Building & Running
1. Clone this repository:
   ```bash
   git clone https://github.com/arahagikoa/space_sim/.git
   cd black_hole/
   ```

2. Compile the simulation:

   ```bash
   g++ -std=c++17 main.cpp engine.cpp black_hole.cpp ray.cpp \
    -lGLEW -lglfw -lGL -o black_hole_sim
   ```
3. Run:

   ```bash
   ./black_hole_sim
   ```

---

## Inspiration

This project was heavily inspired by a YouTube video showcasing how black hole simulations can be implemented from scratch using ray tracing and physics-based light bending. [YT link](https://www.youtube.com/watch?v=8-B6ryuBkCM)

---

##  Roadmap

* 2D black hole gravitational lensing simulation
* 3D ray tracing for black hole visualization <----
* Configurable parameters (mass, spin, observer position)
* Additional celestial objects and scene rendering

---

## License

This project is open-source under the MIT License. Feel free to use, modify, and share it.
