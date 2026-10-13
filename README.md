# 🦅 Interactive 3D Scene — OpenGL

An interactive 3D graphics project developed with **C++** and **OpenGL**. Explore the scene with a controllable camera, animate a 3D model and an eagle, and experiment with rain, fog, and lighting effects.

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++20">
  <img src="https://img.shields.io/badge/OpenGL-3D%20Graphics-5586A4?style=for-the-badge&logo=opengl&logoColor=white" alt="OpenGL">
  <img src="https://img.shields.io/badge/Platform-Windows-0078D4?style=for-the-badge&logo=windows&logoColor=white" alt="Windows">
</p>

## ✨ Features

- **Interactive 3D scene** rendered with OpenGL
- **First-person camera** controlled with the keyboard and mouse
- **Animated eagle** that can be positioned and resized before flight
- **Rain effect** with thousands of particles and wind movement
- **Adjustable fog** to add depth and atmosphere
- **Multiple light sources**, including a point light and a rotating directional light
- **3D model animation** that can be toggled on and off
- **Presentation mode** with an automatically moving camera
- **Multiple rendering modes** for viewing the scene

## 🧰 Built With

- **C++20**
- **OpenGL**
- **GLFW** — window creation and input
- **GLEW** — OpenGL extension loading
- **GLM** — vectors and matrices
- **GLSL** — vertex and fragment shaders
- **stb_image** — image loading
- **tinyobjloader** — OBJ model loading

## 🎮 Controls

### Camera

| Key | Action |
|---|---|
| `W` `A` `S` `D` | Move the camera |
| Mouse | Look around |
| `Space` | Move up |
| `Left Ctrl` | Move down |
| `Esc` | Close the application |

### Scene and Effects

| Key | Action |
|---|---|
| `P` | Toggle presentation camera mode |
| `O` | Toggle 3D model animation |
| `F` | Toggle fog |
| `[` / `]` | Decrease / increase fog density |
| `Y` | Toggle rain |
| `L` | Cycle through point-light modes |
| `N` | Toggle rotation of the second directional light |
| `Q` / `E` | Rotate the animated object |

### Eagle Setup

Use the arrow keys and the controls below to position and resize the eagle. Press `Enter` to start its flight animation.

| Key | Action |
|---|---|
| `←` `→` | Move the eagle horizontally |
| `↑` `↓` | Move the eagle forward or backward |
| `T` / `G` | Move the eagle up / down |
| `Insert` / `Delete` | Increase / decrease the eagle’s size |
| `Enter` | Start the eagle’s flight |

### Rendering Modes

| Key | Action |
|---|---|
| `1` | Wireframe |
| `2` | Smooth shading |
| `3` | Point view |
| `4` | Filled view |

## 🚀 Getting Started

### Requirements

- Windows
- Visual Studio with C++20 support
- OpenGL development libraries: GLFW, GLEW, and GLM

### Run in Visual Studio

1. Clone the repository:

   ```bash
   git clone https://github.com/stefaniaelena20/Scena-3D-in-OpenGL.git
   cd Scena-3D-in-OpenGL
