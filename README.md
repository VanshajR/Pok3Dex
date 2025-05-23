# Pok3Dex

A 3D Pokémon guessing game built with OpenGL, where players try to identify Pokémon from their 3D models.

![Pok3Dex Screenshot](https://res.cloudinary.com/dnciaoigz/image/upload/v1747853097/start_bg_v7g9ay.bmp)

[Download The Game](https://mega.nz/file/wMoUnDiI#GiCdqfaongwvNr6NqOVW6QYnfEhxhxXv7JLCEM5hw5k)

[Download the assets folder](https://mega.nz/file/FMxmEDqI#hf6SdaYyv8EvMqqvOXhUDP38RvWe3mMwGh90g3iCAEs)

## Features

- 3D Pokémon model rendering with real-time rotation
- Interactive guessing system
- Sound effects and background music
- Score tracking and timer system
- Color reveal animation
- Modern UI with custom Pokémon-style font
- Support for all 151 original Pokémon

## Prerequisites

- Windows 10 or later
- Visual Studio 2019 or later
- CMake 3.15 or later
- OpenGL 4.0 or later
- FreeGLUT
- GLEW
- OpenAL
- FreeType
- Assimp
- GLM
- STB Image
- DR WAV

## Installation

1. Clone the repository:
```bash
git clone https://github.com/VanshajR/Pok3Dex.git
cd Pok3Dex
```

2. Install dependencies using vcpkg:
```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
vcpkg install glew:x64-windows freeglut:x64-windows openal-soft:x64-windows freetype:x64-windows assimp:x64-windows glm:x64-windows
```

3. Build the project:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## How to Play

1. Start the game and enter your name
2. A 3D Pokémon model will appear in silhouette
3. Type your guess for the Pokémon's name
4. Press Enter to submit your guess
5. If correct, the model will reveal its colors and you'll move to the next Pokémon
6. If incorrect, the game ends
7. Try to catch all 151 Pokémon!

## Controls

- **Enter**: Start game, confirm name entry, submit guess
- **Backspace**: Delete character in name/guess
- **R**: Restart game (on game over/win screen)
- **Q**: Quit game (on game over/win screen)

## Project Structure

```
Pok3Dex/
├── assets/
│   ├── fonts/
│   ├── models/
│   ├── sounds/
│   └── textures/
├── libs/
│   └── dr_wav.h
├── shaders/
│   ├── vertex.glsl
│   └── fragment.glsl
├── src/
│   └── main.cpp
├── CMakeLists.txt
└── README.md
```

## Troubleshooting

### Common Issues

1. **Missing DLLs**
   - Ensure all required DLLs are in the system PATH or in the same directory as the executable
   - Required DLLs: glew32.dll, freeglut.dll, OpenAL32.dll

2. **OpenGL Errors**
   - Verify your graphics card supports OpenGL 4.0 or later
   - Update graphics drivers

3. **Sound Issues**
   - Check if OpenAL is properly installed
   - Verify sound device is working

4. **Model Loading Issues**
   - Ensure all model files are in the correct format (.obj)
   - Check if texture paths are correct

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the BSD License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Pokémon models and assets are property of Nintendo/Game Freak
- OpenGL community for documentation and support
- All open-source libraries used in this project

## Contact

For questions or support, please open an issue in the GitHub repository. 
