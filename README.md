# Space Simulations

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
You’ll need:
- C++17 or newer
- [CMake](https://cmake.org/download/)
- [vcpkg](https://github.com/microsoft/vcpkg)

---

## Installing Dependencies with vcpkg

1. **Clone vcpkg** (if not already installed):
   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh      # Linux/macOS
   .\bootstrap-vcpkg.bat     # Windows
   ```
2. **Install required libraries**:
   ```bash
   ./vcpkg install glew glfw3 glm
   ```
3.  **Integrate vcpkg with CMake**:
   ```bash
   ./vcpkg integrate install
   ``` 
---

## Building & Running
1. Clone this repository:
   ```bash
   git clone https://github.com/arahagikoa/space_sim/.git
   cd black_hole/
   ```

2. Create a build folder:

   ```bash
    mkdir build
    cd build
   ```
3. Run CMake:

   ```bash
   cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```
4. Build:
   ```bash
   cmake --build .
   ```
5. Run:
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
