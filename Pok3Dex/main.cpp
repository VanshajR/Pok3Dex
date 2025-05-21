#define _CRT_SECURE_NO_WARNINGS

#define DR_WAV_IMPLEMENTATION
#include "./libs/dr_wav.h"


// STB Configuration
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb_image_write.h"

// Add Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/mesh.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <random>           // For random_device and mt19937
#include <chrono>          // For high_resolution_clock
#include <tiny_gltf.h>
#include <fstream>
#include <sstream>
#include <unordered_map>

// ===============================
// Pok3Dex Main Game Source File
// ===============================
// This file contains the main logic, rendering, and game loop for Pok3Dex.
// It is heavily commented for OpenGL beginners and game developers.
//
// Key sections:
//   - Window and OpenGL context setup
//   - Game state and logic
//   - 3D model and camera controls
//   - UI and text rendering
//   - Sound/music management
//   - Detailed function headers and tweakable values
// ===============================

// ===== GAME CONFIGURATION CONSTANTS =====
// These constants control the basic game settings
// Modify these values to change core game behavior

// Window dimensions - Change these to modify the game window size
const int WIDTH = 1280;  // Window width in pixels
const int HEIGHT = 720;  // Window height in pixels

// Animation and timing constants
const float FLASH_SPEED = 0.02f;        // Controls how fast text flashes (higher = faster)
const int TIMER_LIMIT = 60;             // Time limit for each guess in seconds
const float COLOR_REVEAL_TIME = 1.5f;   // How long the color reveal animation lasts

// Game states - Used to control game flow
enum GameState { START_SCREEN, NAME_ENTRY, PLAYING, GAME_OVER, WIN_SCREEN };
GameState gameState = START_SCREEN;

// Function declarations
void playSound(ALuint source);
void drawStartScreen();
void drawNameEntryScreen();
void drawGameScreen();
void drawWinScreen();
void drawGameOverScreen();
void drawInputBox();
void loadModel(int id);
void checkGuess();
void initializePokemonSequence();
int getNextPokemonID();

// Pokémon Data
std::unordered_map<int, std::string> pokemonNames = {
    {1, "BULBASAUR"}, {2, "IVYSAUR"}, {3, "VENUSAUR"},
    {4, "CHARMANDER"}, {5, "CHARMELEON"}, {6, "CHARIZARD"},
    {7, "SQUIRTLE"}, {8, "WARTORTLE"}, {9, "BLASTOISE"},
    {10, "CATERPIE"}, {11, "METAPOD"}, {12, "BUTTERFREE"},
    {13, "WEEDLE"}, {14, "KAKUNA"}, {15, "BEEDRILL"},
    {16, "PIDGEY"}, {17, "PIDGEOTTO"}, {18, "PIDGEOT"},
    {19, "RATTATA"}, {20, "RATICATE"}, {21, "SPEAROW"},
    {22, "FEAROW"}, {23, "EKANS"}, {24, "ARBOK"},
    {25, "PIKACHU"}, {26, "RAICHU"}, {27, "SANDSHREW"},
    {28, "SANDSLASH"}, {29, "NIDORAN F"}, {30, "NIDORINA"},
    {31, "NIDOQUEEN"}, {32, "NIDORAN M"}, {33, "NIDORINO"},
    {34, "NIDOKING"}, {35, "CLEFAIRY"}, {36, "CLEFABLE"},
    {37, "VULPIX"}, {38, "NINETALES"}, {39, "JIGGLYPUFF"},
    {40, "WIGGLYTUFF"}, {41, "ZUBAT"}, {42, "GOLBAT"},
    {43, "ODDISH"}, {44, "GLOOM"}, {45, "VILEPLUME"},
    {46, "PARAS"}, {47, "PARASECT"}, {48, "VENONAT"},
    {49, "VENOMOTH"}, {50, "DIGLETT"}, {51, "DUGTRIO"},
    {52, "MEOWTH"}, {53, "PERSIAN"}, {54, "PSYDUCK"},
    {55, "GOLDUCK"}, {56, "MANKEY"}, {57, "PRIMEAPE"},
    {58, "GROWLITHE"}, {59, "ARCANINE"}, {60, "POLIWAG"},
    {61, "POLIWHIRL"}, {62, "POLIWRATH"}, {63, "ABRA"},
    {64, "KADABRA"}, {65, "ALAKAZAM"}, {66, "MACHOP"},
    {67, "MACHOKE"}, {68, "MACHAMP"}, {69, "BELLSPROUT"},
    {70, "WEEPINBELL"}, {71, "VICTREEBEL"}, {72, "TENTACOOL"},
    {73, "TENTACRUEL"}, {74, "GEODUDE"}, {75, "GRAVELER"},
    {76, "GOLEM"}, {77, "PONYTA"}, {78, "RAPIDASH"},
    {79, "SLOWPOKE"}, {80, "SLOWBRO"}, {81, "MAGNEMITE"},
    {82, "MAGNETON"}, {83, "FARFETCH'D"}, {84, "DODUO"},
    {85, "DODRIO"}, {86, "SEEL"}, {87, "DEWGONG"},
    {88, "GRIMER"}, {89, "MUK"}, {90, "SHELLDER"},
    {91, "CLOYSTER"}, {92, "GASTLY"}, {93, "HAUNTER"},
    {94, "GENGAR"}, {95, "ONIX"}, {96, "DROWZEE"},
    {97, "HYPNO"}, {98, "KRABBY"}, {99, "KINGLER"},
    {100, "VOLTORB"}, {101, "ELECTRODE"}, {102, "EXEGGCUTE"},
    {103, "EXEGGUTOR"}, {104, "CUBONE"}, {105, "MAROWAK"},
    {106, "HITMONLEE"}, {107, "HITMONCHAN"}, {108, "LICKITUNG"},
    {109, "KOFFING"}, {110, "WEEZING"}, {111, "RHYHORN"},
    {112, "RHYDON"}, {113, "CHANSEY"}, {114, "TANGELA"},
    {115, "KANGASKHAN"}, {116, "HORSEA"}, {117, "SEADRA"},
    {118, "GOLDEEN"}, {119, "SEAKING"}, {120, "STARYU"},
    {121, "STARMIE"}, {122, "MR MIME"}, {123, "SCYTHER"},
    {124, "JYNX"}, {125, "ELECTABUZZ"}, {126, "MAGMAR"},
    {127, "PINSIR"}, {128, "TAUROS"}, {129, "MAGIKARP"},
    {130, "GYARADOS"}, {131, "LAPRAS"}, {132, "DITTO"},
    {133, "EEVEE"}, {134, "VAPOREON"}, {135, "JOLTEON"},
    {136, "FLAREON"}, {137, "PORYGON"}, {138, "OMANYTE"},
    {139, "OMASTAR"}, {140, "KABUTO"}, {141, "KABUTOPS"},
    {142, "AERODACTYL"}, {143, "SNORLAX"}, {144, "ARTICUNO"},
    {145, "ZAPDOS"}, {146, "MOLTRES"}, {147, "DRATINI"},
    {148, "DRAGONAIR"}, {149, "DRAGONITE"}, {150, "MEWTWO"},
    {151, "MEW"}
};

// ===== POKEMON SEQUENCE CONTROL =====
// These variables control the order of Pokémon appearance
std::vector<int> availablePokemon;    // List of Pokémon IDs that haven't been shown yet
std::vector<int> usedPokemon;         // List of Pokémon IDs that have been shown
bool isFirstGame = true;              // Track if this is the first game

// ===============================
// Function: initializePokemonSequence
// Purpose: Initializes and shuffles the Pokémon sequence for the game.
// ===============================

// Function to initialize or reset the Pokémon sequence
void initializePokemonSequence() {
    // Clear existing sequences
    availablePokemon.clear();
    usedPokemon.clear();

    // Add all Pokémon IDs (1-151) to available list
    for (int i = 1; i <= 151; i++) {
        availablePokemon.push_back(i);
    }

    // Shuffle the available Pokémon
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(availablePokemon.begin(), availablePokemon.end(), g);
}

// ===============================
// Function: getNextPokemonID
// Purpose: Returns the next Pokémon ID from the shuffled sequence.
// ===============================

// Function to get next Pokémon ID
int getNextPokemonID() {
    // If all Pokémon have been used, reset the sequence
    if (availablePokemon.empty()) {
        // Move all used Pokémon back to available
        availablePokemon = usedPokemon;
        usedPokemon.clear();

        // Shuffle the available Pokémon again
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(availablePokemon.begin(), availablePokemon.end(), g);
    }

    // Get the next Pokémon ID
    int nextID = availablePokemon.back();
    availablePokemon.pop_back();
    usedPokemon.push_back(nextID);

    return nextID;
}

// Structures
struct Character {
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};
std::map<char, Character> characters;

struct MeshData {
    GLuint vao, vbo, ibo, tbo, nbo;
    size_t indexCount;
    int materialIndex;
};
struct ModelData {
    std::vector<MeshData> meshes;
    std::vector<GLuint> textures;
    std::vector<GLuint> materialToTexture;
    std::unique_ptr<Assimp::Importer> importer;
    const aiScene* scene = nullptr;
    std::vector<aiMaterial*> materials;
};
std::unordered_map<int, ModelData> pokemonModels;

// Remove tinygltf::Model currentModel since we're using Assimp
struct Texture {
    GLuint id;
    int width;
    int height;
};
Texture startBg, gameBg, pokeballTex;

// Sound
ALCdevice* alDevice;
ALCcontext* alContext;
ALuint bgmSource, successSource, failureSource, gameBgmSource, gameOverSource, winnerBgmSource;

// ===== GAME STATE VARIABLES =====
// These variables control the current game state
// Modify these to change how the game behaves during play

// Player input and display
std::string playerName;      // Player's name
std::string currentGuess;    // Current Pokémon name guess
int currentPokemonID;        // Current Pokémon being displayed (1-151)
int score = 0;               // Player's current score

// Visual effects
float flashAlpha = 1.0f;     // Controls text flash opacity (0.0-1.0)
bool flashDir = false;       // Controls flash direction (true = fading in)
float rotationAngle = 0.0f;  // Controls model rotation angle
bool modelLoaded = false;    // Tracks if current model is loaded

// Game timing
int remainingTime = TIMER_LIMIT;    // Time remaining for current guess
bool isColorReveal = false;         // Whether color reveal is active
float colorRevealTimer = 0.0f;      // Tracks color reveal animation progress
bool isSilhouette = true;           // Whether model is in silhouette mode

// ===== RENDERING SETTINGS =====
// These settings control how the game looks
// Modify these to change visual appearance

// Texture pool for managing game textures
const int TEXTURE_POOL_SIZE = 100;  // Maximum number of textures
GLuint texturePool[TEXTURE_POOL_SIZE];
int nextTexturePoolIndex = 0;

// Shader helpers
GLuint shaderProgram;

// ===============================
// Function: loadShaderSource
// Purpose: Loads shader source code from a file.
// Parameters: filePath - path to the shader file.
// Returns: String containing the shader source code.
// ===============================

// Function: loadShaderSource
// Purpose: Loads shader source code from a file
// Parameters:
//   - filePath: Path to the shader file
// Returns: String containing the shader source code
std::string loadShaderSource(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ===============================
// Function: compileShader
// Purpose: Compiles a shader from source code.
// Parameters: type (GL_VERTEX_SHADER/GL_FRAGMENT_SHADER), source (shader code)
// Returns: Compiled shader ID.
// ===============================

// Function: compileShader
// Purpose: Compiles a shader from source code
// Parameters:
//   - type: Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
//   - source: Source code of the shader
// Returns: Compiled shader ID
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

// ===============================
// Function: createShaderProgram
// Purpose: Creates a complete shader program from vertex and fragment shaders.
// Parameters: vertexPath, fragmentPath - paths to shader files.
// Returns: Linked shader program ID.
// ===============================

// Function: createShaderProgram
// Purpose: Creates a complete shader program from vertex and fragment shaders
// Parameters:
//   - vertexPath: Path to vertex shader file
//   - fragmentPath: Path to fragment shader file
// Returns: Linked shader program ID
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertSource = loadShaderSource(vertexPath);
    std::string fragSource = loadShaderSource(fragmentPath);
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertSource.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragSource.c_str());
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed: " << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

// ===============================
// Function: setTextureParameters
// Purpose: Sets up texture parameters for OpenGL textures.
// Parameters: texture - OpenGL texture ID to configure.
// ===============================

// Function: setTextureParameters
// Purpose: Sets up texture parameters for OpenGL textures
// Parameters:
//   - texture: OpenGL texture ID to configure
void setTextureParameters(GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

// ===============================
// Function: loadTexture
// Purpose: Loads a texture from an image file.
// Parameters: path - path to the texture image file.
// Returns: OpenGL texture ID.
// ===============================

// Function: loadTexture
// Purpose: Loads a texture from an image file
// Parameters:
//   - path: Path to the texture image file
// Returns: OpenGL texture ID
// Note: Can be modified to change texture loading behavior
GLuint loadTexture(const char* path) {
    stbi_set_flip_vertically_on_load(true);
    int w, h, ch;
    unsigned char* data = stbi_load(path, &w, &h, &ch, 4);

    if (!data) {
        std::cerr << "ERROR: Failed to load texture at " << path
            << "\nReason: " << stbi_failure_reason() << std::endl;
        return 0;
    }

    if (w <= 0 || h <= 0) {
        std::cerr << "Invalid texture dimensions for " << path << "!" << std::endl;
        stbi_image_free(data);
        return 0;
    }

    std::cout << "Loaded image: " << path << " (" << w << "x" << h << ", " << ch << " channels)" << std::endl;

    GLuint tex;
    glGenTextures(1, &tex);
    if (tex == 0) {
        std::cerr << "Failed to generate texture ID for " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    setTextureParameters(tex);

    stbi_image_free(data);
    return tex;
}

// ===============================
// Function: renderText
// Purpose: Renders text using FreeType fonts.
// Parameters: text, x, y, scale, color.
// ===============================

// Function: renderText
// Purpose: Renders text using FreeType fonts
// Parameters:
//   - text: Text to render
//   - x, y: Screen coordinates
//   - scale: Text size multiplier
//   - color: RGB color values
void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);  // Save color state
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Get current color and alpha
    GLfloat currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor);

    for (auto c = text.begin(); c != text.end(); c++) {
        Character ch = characters[*c];
        float xpos = x + ch.bearing.x * scale;
        float ypos = y + (ch.size.y - ch.bearing.y) * scale;
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glColor4f(color.r, color.g, color.b, currentColor[3]);  // Use current alpha
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(xpos, ypos);
        glTexCoord2f(1, 0); glVertex2f(xpos + ch.size.x * scale, ypos);
        glTexCoord2f(1, 1); glVertex2f(xpos + ch.size.x * scale, ypos - ch.size.y * scale);
        glTexCoord2f(0, 1); glVertex2f(xpos, ypos - ch.size.y * scale);
        glEnd();
        x += (ch.advance >> 6) * scale;
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glDisable(GL_BLEND);
    glPopAttrib();  // Restore color state
}

// ===============================
// Function: loadFont
// Purpose: Loads a font using FreeType and creates OpenGL textures for each glyph.
// Parameters: path - path to the .ttf font file.
// ===============================

void loadFont(const char* path) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Failed to initialize FreeType" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        FT_Done_FreeType(ft);
        return;
    }

    // Set font size
    FT_Set_Pixel_Sizes(face, 0, 64);

    // Clear existing characters
    characters.clear();

    // Load all ASCII characters
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph " << c << std::endl;
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Convert bitmap to RGBA
        GLubyte* rgbaData = new GLubyte[face->glyph->bitmap.width * face->glyph->bitmap.rows * 4];
        for (int i = 0; i < face->glyph->bitmap.width * face->glyph->bitmap.rows; i++) {
            rgbaData[i * 4] = 255;     // R
            rgbaData[i * 4 + 1] = 255; // G
            rgbaData[i * 4 + 2] = 255; // B
            rgbaData[i * 4 + 3] = face->glyph->bitmap.buffer[i]; // A
        }

        // Upload texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);

        delete[] rgbaData;

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store character
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        characters[c] = character;
    }

    // Clean up
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Verify font loading
    if (characters.empty() || characters.find('A') == characters.end()) {
        std::cerr << "ERROR::FREETYPE: Font loading verification failed" << std::endl;
    }
    else {
        std::cout << "Font loaded successfully!" << std::endl;
    }
}

// ===============================
// Function: initSound
// Purpose: Loads all sound files and initializes OpenAL sources for BGM and SFX.
// ===============================

void initSound() {
    alDevice = alcOpenDevice(nullptr);
    alContext = alcCreateContext(alDevice, nullptr);
    alcMakeContextCurrent(alContext);

    auto loadSound = [](const char* path) -> ALuint {
        drwav wav;
        if (!drwav_init_file(&wav, path, nullptr)) return 0;

        std::vector<int16_t> pcmData(wav.totalPCMFrameCount * wav.channels);
        drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcmData.data());

        ALuint buffer;
        alGenBuffers(1, &buffer);
        alBufferData(buffer, wav.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
            pcmData.data(), pcmData.size() * sizeof(int16_t), wav.sampleRate);

        drwav_uninit(&wav);
        return buffer;
        };

    ALuint bgmBuffer = loadSound("assets/sounds/bgm.wav");
    ALuint successBuffer = loadSound("assets/sounds/success.wav");
    ALuint failureBuffer = loadSound("assets/sounds/failure.wav");
    ALuint gameBgmBuffer = loadSound("assets/sounds/guess_bgm.wav");
    ALuint gameOverBuffer = loadSound("assets/sounds/game_over.wav");
    ALuint winnerBgmBuffer = loadSound("assets/sounds/winner_bgm.wav");

    alGenSources(1, &bgmSource);
    alSourcei(bgmSource, AL_BUFFER, bgmBuffer);
    alSourcei(bgmSource, AL_LOOPING, AL_TRUE);

    alGenSources(1, &gameBgmSource);
    alSourcei(gameBgmSource, AL_BUFFER, gameBgmBuffer);
    alSourcei(gameBgmSource, AL_LOOPING, AL_TRUE);

    alGenSources(1, &successSource);
    alSourcei(successSource, AL_BUFFER, successBuffer);

    alGenSources(1, &failureSource);
    alSourcei(failureSource, AL_BUFFER, failureBuffer);

    alGenSources(1, &gameOverSource);
    alSourcei(gameOverSource, AL_BUFFER, gameOverBuffer);

    alGenSources(1, &winnerBgmSource);
    alSourcei(winnerBgmSource, AL_BUFFER, winnerBgmBuffer);
    alSourcei(winnerBgmSource, AL_LOOPING, AL_TRUE);

    alSourcePlay(bgmSource);
}

// ===============================
// Function: loadModel
// Purpose: Loads a 3D model for a Pokémon using Assimp.
// Parameters: id - Pokémon ID (1-151).
// Notes: Controls model scaling, camera, and material/texture setup.
// ===============================

// Function: loadModel
// Purpose: Loads a 3D model for a Pokémon
// Parameters:
//   - id: Pokémon ID (1-151)
// Note: Can be modified to change model loading behavior and scaling
void loadModel(int id) {
    // ===== MODEL LOADING AND SCALING CONTROL =====
    // Modify these values to change how models are loaded and displayed

    float modelH = 0.0f;  // Will be set after computing bounding box
    float containerH = 0.5f;              // Target height for model container (world units)
    float scale = containerH / modelH;     // Scale factor to fit model in container

    // Camera settings - Controls view of the model
    float fovY = 45.0f;                   // Field of view angle (degrees)
    float fovY_rad = glm::radians(fovY);  // Convert to radians
    float camDist = (containerH * 0.5f) / tan(fovY_rad * 0.5f);
    camDist *= 2.0f;                      // Camera distance multiplier

    // Model position adjustment
    float modelYOffset = -0.02f;          // Vertical offset of model (world units)
    float modelBaseY = -0.05f;            // Base Y position of model (world units)

    // Clear existing model data
    if (pokemonModels.find(id) != pokemonModels.end()) {
        // Unbind everything before deletion
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        for (const auto& mesh : pokemonModels[id].meshes) {
            glDeleteVertexArrays(1, &mesh.vao);
            glDeleteBuffers(1, &mesh.vbo);
            glDeleteBuffers(1, &mesh.ibo);
            glDeleteBuffers(1, &mesh.tbo);
            glDeleteBuffers(1, &mesh.nbo);
        }
        // Delete textures
        for (GLuint texture : pokemonModels[id].textures) {
            if (texture != 0) {
                glDeleteTextures(1, &texture);
            }
        }
        glFinish();
        pokemonModels.erase(id);
    }

    std::string folder = std::to_string(id);
    folder = std::string(3 - folder.length(), '0') + folder;
    std::string basePath = "assets/models/" + folder + "/";
    std::string path = basePath + "model.obj";

    std::cout << "Loading model from: " << path << std::endl;
    std::cout << "Base path for materials: " << basePath << std::endl;

    ModelData modelData;
    modelData.importer = std::make_unique<Assimp::Importer>();
    modelData.scene = modelData.importer->ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    const aiScene* scene = modelData.scene;
    if (!scene) {
        std::cerr << "Failed to load model: " << modelData.importer->GetErrorString() << std::endl;
        return;
    }

    std::cout << "Loaded " << scene->mNumMeshes << " meshes and "
        << scene->mNumMaterials << " materials" << std::endl;

    // Process materials and textures
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        aiString texPath;
        GLuint texture = 0;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
            std::string texFile = texPath.C_Str();
            size_t lastSlash = texFile.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                texFile = texFile.substr(lastSlash + 1);
            }
            std::string fullPath = basePath + texFile;
            std::cout << "Loading texture: " << fullPath << std::endl;
            texture = loadTexture(fullPath.c_str());
        }
        modelData.textures.push_back(texture);
        modelData.materialToTexture.push_back(texture);
        modelData.materials.push_back(material);
    }
    std::cout << "Material to Texture mapping: ";
    for (size_t i = 0; i < modelData.materialToTexture.size(); ++i) {
        std::cout << "[" << i << "]=" << modelData.materialToTexture[i] << " ";
    }
    std::cout << std::endl;

    // --- Compute bounding box for all meshes ---
    glm::vec3 minBB(FLT_MAX), maxBB(-FLT_MAX);
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            glm::vec3 v(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
            minBB = glm::min(minBB, v);
            maxBB = glm::max(maxBB, v);
        }
    }
    glm::vec3 size = maxBB - minBB;
    glm::vec3 center = (minBB + maxBB) * 0.5f;
    modelH = size.y;
    scale = containerH / modelH;
    // --- End bounding box ---

    // --- Per-mesh VAO/VBO/IBO/TBO/NBO ---
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        MeshData meshData;
        meshData.materialIndex = mesh->mMaterialIndex;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<unsigned int> indices;
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            glm::vec3 v(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
            v = (v - center) * scale;
            vertices.emplace_back(v);
            if (mesh->HasNormals())
                normals.emplace_back(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
            if (mesh->HasTextureCoords(0))
                texCoords.emplace_back(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
        }
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k)
                indices.push_back(face.mIndices[k]);
        }
        meshData.indexCount = indices.size();
        // Create and bind VAO
        glGenVertexArrays(1, &meshData.vao);
        glBindVertexArray(meshData.vao);
        // VBO
        glGenBuffers(1, &meshData.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        // TBO (texcoords)
        glGenBuffers(1, &meshData.tbo);
        glBindBuffer(GL_ARRAY_BUFFER, meshData.tbo);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        // NBO (normals)
        glGenBuffers(1, &meshData.nbo);
        glBindBuffer(GL_ARRAY_BUFFER, meshData.nbo);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);
        // IBO
        glGenBuffers(1, &meshData.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        modelData.meshes.push_back(meshData);
    }
    pokemonModels[id] = std::move(modelData);
    modelLoaded = true;
    isColorReveal = false;
    colorRevealTimer = 0.0f;

    std::cout << "Model loaded successfully with " << modelData.textures.size() << " textures" << std::endl;

    // Print OpenGL version and vendor
    const GLubyte* glVersion = glGetString(GL_VERSION);
    const GLubyte* glVendor = glGetString(GL_VENDOR);
    std::cout << "OpenGL Version: " << (glVersion ? (const char*)glVersion : "(null)") << std::endl;
    std::cout << "OpenGL Vendor: " << (glVendor ? (const char*)glVendor : "(null)") << std::endl;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// ===============================
// Function: drawGameScreen
// Purpose: Renders the main game screen, including 3D model, UI, and overlays.
// Notes: Controls camera, viewport, and all 2D/3D drawing for gameplay.
// ===============================

void drawGameScreen() {
    // ===== UI LAYOUT CONTROL =====
    // Modify these values to change the position and size of UI elements

    // Guess input box dimensions and position
    float guessBoxWidth = WIDTH - 200;    // Width of guess input box (pixels)
    float guessBoxHeight = 90;            // Height of guess input box (pixels)
    float guessBoxX = 100;                // X position from left edge (pixels)
    float guessBoxY = 30;                 // Y position from bottom (pixels)

    // Text scaling factors - Controls size of different text elements
    float guessLabelScale = 0.8f;         // Scale of "GUESS:" label
    float guessScale = 0.8f;              // Scale of guess text
    float scoreScale = 0.7f;              // Scale of score display
    float timeScale = 0.7f;               // Scale of timer display

    // Player info position
    float playerInfoX = 40;               // X position of player name (pixels)
    float playerInfoY = HEIGHT - 60;      // Y position of player name (pixels)

    // Top right: Score and Timer
    std::string scoreStr = "SCORE: " + std::to_string(score);
    std::string timeStr = "TIME: " + std::to_string(remainingTime);
    float scoreWidth = scoreStr.length() * 32 * scoreScale * 0.6f;
    float timeWidth = timeStr.length() * 32 * timeScale * 0.6f;
    float scoreX = WIDTH - scoreWidth - 40;    // X position of score (pixels)
    float scoreY = HEIGHT - 60;                // Y position of score (pixels)
    float timeX = WIDTH - timeWidth - 40;      // X position of timer (pixels)
    float timeY = HEIGHT - 110;                // Y position of timer (pixels)

    // Clear the screen and set up 2D rendering for background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw game background
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gameBg.id);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(WIDTH, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(WIDTH, HEIGHT);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, HEIGHT);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // 3D model rendering
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WIDTH / HEIGHT, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float containerH = 0.5f;
    float fovY = 45.0f;
    float fovY_rad = glm::radians(fovY);
    float camDist = (containerH * 0.5f) / tan(fovY_rad * 0.5f);
    camDist *= 2.0f;
    // Move model base slightly further down (closer to center)
    gluLookAt(0, -0.05, camDist, 0, -0.05, 0, 0, 1, 0);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    if (modelLoaded && pokemonModels.count(currentPokemonID)) {
        const ModelData& modelData = pokemonModels[currentPokemonID];
        glUseProgram(shaderProgram);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0, 1, 0));
        // Move model slightly further down in world space
        model = glm::translate(model, glm::vec3(0, -0.02f, 0));
        glm::mat4 view = glm::lookAt(glm::vec3(0, -0.05, camDist), glm::vec3(0, -0.05, 0), glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        for (const auto& meshData : modelData.meshes) {
            int matIndex = meshData.materialIndex;
            if (matIndex >= 0 && matIndex < modelData.materialToTexture.size()) {
                GLuint tex = modelData.materialToTexture[matIndex];
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex);
                glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
            }
            glBindVertexArray(meshData.vao);
            glDrawElements(GL_TRIANGLES, meshData.indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        glUseProgram(0);
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    // 2D UI overlay
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Top left: Player name
    renderText("PLAYER: " + playerName, playerInfoX, playerInfoY, 0.7f, { 1,1,1 });
    // Top right: Score and Timer
    renderText(scoreStr, scoreX, scoreY, scoreScale, { 1,1,1 });
    renderText(timeStr, timeX, timeY, timeScale, { 1,1,1 });
    // 'GUESS:' left-aligned in rectangle, vertically centered
    std::string guessLabel = "GUESS:";
    float guessLabelX = guessBoxX + 20;
    float fontPixelHeight = characters['A'].size.y * guessLabelScale;
    float centerY = guessBoxY + guessBoxHeight / 2;
    float guessLabelY = centerY + 10;
    renderText(guessLabel, guessLabelX, guessLabelY, guessLabelScale, { 1,1,1 }); // Always white
    // User's guess centered in rectangle (not window), vertically centered
    std::string guessMsg = currentGuess;
    float guessWidth = guessMsg.length() * 32 * guessScale * 0.6f;
    float guessCenterX = guessBoxX + guessBoxWidth / 2 - guessWidth / 2;
    float guessCenterY = centerY + 10;
    renderText(guessMsg, guessCenterX, guessCenterY, guessScale, { 1,1,1 }); // Always white
}

// ===============================
// Function: drawGameOverScreen
// Purpose: Renders the game over screen with score and restart/quit options.
// ===============================

void drawGameOverScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    std::string overMsg = "GAME OVER";
    float overScale = 2.0f; // larger
    float overWidth = overMsg.length() * 32 * overScale * 0.6f;
    renderText(overMsg, WIDTH / 2 - overWidth / 2, HEIGHT / 2 + 200, overScale, { 1,1,1 }); // much higher
    std::string finalScore = playerName + ",S FINAL SCORE: " + std::to_string(score);
    float scoreScale = 1.0f;
    float scoreWidth = finalScore.length() * 32 * scoreScale * 0.6f;
    renderText(finalScore, WIDTH / 2 - scoreWidth / 2, HEIGHT / 2 + 10, scoreScale, { 1,1,1 });
    std::string restartMsg = "PRESS R TO RESTART";
    float restartScale = 0.7f;
    float restartWidth = restartMsg.length() * 32 * restartScale * 0.6f;
    renderText(restartMsg, WIDTH / 2 - restartWidth / 2, HEIGHT / 2 - 60, restartScale, { 1,1,1 });
    std::string quitMsg = "PRESS Q TO QUIT";
    float quitScale = 0.7f;
    float quitWidth = quitMsg.length() * 32 * quitScale * 0.6f;
    renderText(quitMsg, WIDTH / 2 - quitWidth / 2, HEIGHT / 2 - 110, quitScale, { 1,1,1 });
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// ===============================
// Function: drawWinScreen
// Purpose: Renders the win screen with a pretty yellow background and winner BGM.
// ===============================

void drawWinScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set orange background
    glClearColor(1.0f, 0.65f, 0.0f, 1.0f);  // Changed from yellow to orange
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    std::string congrats = "CONGRATULATIONS " + playerName + "' YOU,RE A POKEMON MASTER!";
    float congratsScale = 1.0f;
    float congratsWidth = congrats.length() * 32 * congratsScale * 0.6f;
    renderText(congrats, WIDTH / 2 - congratsWidth / 2, HEIGHT / 2 + 40, congratsScale, { 1,1,1 });
    std::string complete = "GAME COMPLETE";
    float completeScale = 1.2f;
    float completeWidth = complete.length() * 32 * completeScale * 0.6f;
    renderText(complete, WIDTH / 2 - completeWidth / 2, HEIGHT / 2 - 40, completeScale, { 1,1,1 });
    std::string restartMsg = "PRESS R TO RESTART";
    float restartScale = 0.7f;
    float restartWidth = restartMsg.length() * 32 * restartScale * 0.6f;
    renderText(restartMsg, WIDTH / 2 - restartWidth / 2, HEIGHT / 2 - 120, restartScale, { 1,1,1 });
    std::string quitMsg = "PRESS Q TO QUIT";
    float quitScale = 0.7f;
    float quitWidth = quitMsg.length() * 32 * quitScale * 0.6f;
    renderText(quitMsg, WIDTH / 2 - quitWidth / 2, HEIGHT / 2 - 170, quitScale, { 1,1,1 });
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// ===============================
// Function: checkGuess
// Purpose: Validates the player's guess, updates score, and handles win/game over.
// Notes: Change the win condition here (score == 151 for full game, or 2 for test).
// ===============================

// Function: checkGuess
// Purpose: Validates player's Pokémon guess
// Note: Can be modified to change scoring and game progression
void checkGuess() {
    // ===== TEMPORARY CHEAT FOR TESTING =====
    // If the guess is 'E', auto-fill the correct answer for testing
    // Remove this block to restore normal behavior
    if (currentGuess == "CHEAT") {
        currentGuess = pokemonNames[currentPokemonID];
    }
    // ===== END CHEAT BLOCK =====

    // ===== GAME LOGIC CONTROL =====
    // Modify these values to change game rules and progression

    // Convert guess to uppercase and remove spaces
    std::string upperGuess;
    for (char c : currentGuess) upperGuess += toupper(c);
    // Don't remove spaces anymore since some Pokémon names have spaces
    // upperGuess.erase(std::remove_if(upperGuess.begin(), upperGuess.end(), ::isspace), upperGuess.end());

    // Check if guess is correct
    if (pokemonNames[currentPokemonID] == upperGuess) {
        score++;                          // Increment score for correct guess

        // Win condition check (caught all 151 Pokémon)
        if (score == 151) {
            gameState = WIN_SCREEN;
            alSourceStop(gameBgmSource);
            alSourcePlay(winnerBgmSource);
            glutPostRedisplay();
            return;
        }

        // Start color reveal animation
        isColorReveal = true;
        colorRevealTimer = 0.0f;
        currentGuess.clear();

        // Pause game BGM and play success sound
        alSourcePause(gameBgmSource);
        playSound(successSource);
    }
    else {
        // Wrong guess - game over
        gameState = GAME_OVER;
        alSourceStop(gameBgmSource);
        playSound(gameOverSource);
    }
}

// ===============================
// Function: keyboard
// Purpose: Handles all keyboard input for the game, including state transitions.
// Notes: Change input handling here for name entry, guesses, and cheats.
// ===============================

void keyboard(unsigned char key, int x, int y) {
    switch (gameState) {
    case START_SCREEN:
        if (key == 13) {
            gameState = NAME_ENTRY;
            playerName.clear();
        }
        break;
    case NAME_ENTRY:
        if (key == 13) {
            if (!playerName.empty()) {
                gameState = PLAYING;
                // Initialize Pokémon sequence for new game
                initializePokemonSequence();
                currentPokemonID = getNextPokemonID();
                score = 0;
                remainingTime = TIMER_LIMIT;
                isSilhouette = true;
                isColorReveal = false;
                colorRevealTimer = 0.0f;
                loadModel(currentPokemonID);
                alSourceStop(bgmSource);
                alSourcePlay(gameBgmSource);
            }
        }
        else if (key == 8 && !playerName.empty()) {
            playerName.pop_back();
        }
        else if (isalnum(key)) {
            if (playerName.length() < 12)
                playerName += toupper(key);
        }
        break;
    case PLAYING:
        if (key == 13) {
            if (!isColorReveal) {
                checkGuess();
            }
        }
        else if (key == 8 && !currentGuess.empty()) {
            currentGuess.pop_back();
        }
        else if (isalpha(key) || key == ' ' || key == '\'') {  // Added apostrophe check
            currentGuess += toupper(key);
        }
        break;
    case GAME_OVER:
        if (key == 'r' || key == 'R') {
            gameState = START_SCREEN;
            playerName.clear();
            currentGuess.clear();
            remainingTime = TIMER_LIMIT;
            // Reset Pokémon sequence for new game
            initializePokemonSequence();
            // Reset flash variables
            flashAlpha = 1.0f;
            flashDir = false;
            alSourceStop(gameBgmSource);
            alSourcePlay(bgmSource);
        }
        else if (key == 'q' || key == 'Q') {
            exit(0);
        }
        break;
    case WIN_SCREEN:
        if (key == 'r' || key == 'R') {
            gameState = START_SCREEN;
            playerName.clear();
            currentGuess.clear();
            remainingTime = TIMER_LIMIT;
            score = 0;
            // Reset Pokémon sequence for new game
            initializePokemonSequence();
            // Reset flash variables
            flashAlpha = 1.0f;
            flashDir = false;
            alSourceStop(gameBgmSource);
            alSourceStop(winnerBgmSource);
            alSourcePlay(bgmSource);
        }
        else if (key == 'q' || key == 'Q') {
            alSourceStop(winnerBgmSource);
            exit(0);
        }
        break;
    }
    glutPostRedisplay();
}

// ===============================
// Function: update
// Purpose: Main game update loop, handles animations, timing, and model rotation.
// ===============================

// Function: update
// Purpose: Game update loop, handles animations and timing
// Parameters:
//   - value: Timer value (unused)
// Note: Can be modified to change game speed and animations
void update(int value) {
    // ===== ANIMATION AND TIMING CONTROL =====
    // Modify these values to change animation speeds and timing

    // Model rotation
    float rotationSpeed = 1.0f;           // Degrees per frame
    rotationAngle += rotationSpeed;
    if (rotationAngle > 360) rotationAngle -= 360;

    // Game timing
    if (gameState == PLAYING) {
        if (isColorReveal) {
            float revealSpeed = 0.016f;    // Color reveal animation speed
            colorRevealTimer += revealSpeed;

            if (colorRevealTimer >= COLOR_REVEAL_TIME) {
                isColorReveal = false;
                // Get next Pokémon from shuffled sequence
                currentPokemonID = getNextPokemonID();

                // Check if all Pokémon have been caught
                if (score == 151) {
                    gameState = WIN_SCREEN;
                    alSourceStop(gameBgmSource);
                    glutPostRedisplay();
                    return;
                }

                remainingTime = TIMER_LIMIT;
                loadModel(currentPokemonID);

                // Resume game BGM
                alSourcePlay(gameBgmSource);
            }
        }
        else {
            // Timer update
            static float timeAccumulator = 0.0f;
            float timeStep = 0.016f;       // Timer update speed
            timeAccumulator += timeStep;

            if (timeAccumulator >= 1.0f) {
                remainingTime--;
                timeAccumulator = 0.0f;

                // Game over check
                if (remainingTime <= 0) {
                    gameState = GAME_OVER;
                    alSourceStop(gameBgmSource);
                    playSound(gameOverSource);
                }
            }
        }
    }

    // Update display and set next frame
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);         // 60 FPS (1000ms/60 ≈ 16ms)
}

// ===============================
// Function: cleanup
// Purpose: Cleans up OpenGL and OpenAL resources on exit.
// ===============================

// Function: cleanup
// Purpose: Cleans up OpenGL and OpenAL resources
// Note: Called on program exit
void cleanup() {
    // Delete OpenGL resources
    for (auto& pair : pokemonModels) {
        for (const auto& mesh : pair.second.meshes) {
            glDeleteVertexArrays(1, &mesh.vao);
            glDeleteBuffers(1, &mesh.vbo);
            glDeleteBuffers(1, &mesh.ibo);
            glDeleteBuffers(1, &mesh.tbo);
            glDeleteBuffers(1, &mesh.nbo);
        }
        // Delete textures
        for (GLuint texture : pair.second.textures) {
            if (texture != 0) {
                glDeleteTextures(1, &texture);
            }
        }
        glFinish();
    }
    // Do NOT delete textures in the pool (pool workaround)
    // glDeleteTextures(TEXTURE_POOL_SIZE, texturePool);

    // Cleanup OpenAL
    alDeleteSources(1, &bgmSource);
    alDeleteSources(1, &successSource);
    alDeleteSources(1, &failureSource);
    alDeleteSources(1, &gameOverSource);
    alDeleteSources(1, &winnerBgmSource);
    alcDestroyContext(alContext);
    alcCloseDevice(alDevice);
}

// ===============================
// Function: playSound
// Purpose: Plays a given OpenAL sound source if not already playing.
// Parameters: source - ALuint sound source.
// ===============================

void playSound(ALuint source) {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        alSourcePlay(source);
    }
}

// ===============================
// Function: drawInputBox
// Purpose: (Legacy) Draws a GLUT-based input box for guesses (not used in main UI).
// ===============================

void drawInputBox() {
    // GLUT-based input box
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(50, HEIGHT - 200);  // Fix position
    glVertex2f(WIDTH - 50, HEIGHT - 200);
    glVertex2f(WIDTH - 50, HEIGHT - 100);
    glVertex2f(50, HEIGHT - 100);
    glEnd();

    glColor3f(1, 1, 1);
    glRasterPos2f(70, HEIGHT - 150); // Fix text position
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)("GUESS: " + currentGuess).c_str());
}

// ===============================
// Function: drawStartScreen
// Purpose: Renders the start screen, including background and flashing text.
// Notes: Controls text flashing speed and color. Tweak FLASH_SPEED for effect.
// ===============================

void drawStartScreen() {
    glUseProgram(0);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST); // Disable depth test for 2D UI

    // Set a known clear color (same as background)
    glClearColor(0.070f, 0.133f, 0.227f, 1.0f); // dark blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Debug: Print font status
    if (characters.empty() || characters.find('A') == characters.end()) {
        std::cerr << "[drawStartScreen] Font NOT loaded or missing 'A' character!" << std::endl;
        return;
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw background
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, startBg.id);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(WIDTH, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(WIDTH, HEIGHT);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, HEIGHT);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // --- Ensure OpenGL state for text rendering ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Always use flashing logic
    flashAlpha += flashDir ? FLASH_SPEED * 3.0f : -FLASH_SPEED * 3.0f;
    if (flashAlpha >= 1.0f) {
        flashAlpha = 1.0f;
        flashDir = false;
    }
    else if (flashAlpha <= 0.2f) {
        flashAlpha = 0.2f;
        flashDir = true;
    }
    float textAlpha = flashAlpha;

    glColor4f(1.0f, 1.0f, 1.0f, textAlpha); // Set color and alpha explicitly

    // Draw the text
    float textScale = 1.1f;
    std::string msg = "PRESS ENTER TO START";
    float textWidth = msg.length() * 32 * textScale * 0.6f;
    float textX = WIDTH / 2 - textWidth / 2;
    float textY = 60; // 60px above the bottom

    renderText(msg, textX, textY, textScale, { 1,1,1 });

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// ===============================
// Function: drawNameEntryScreen
// Purpose: Renders the name entry screen, including instructions for gender guesses.
// Notes: Adjust Y positions and scales for layout. Tweak colors and spacing as needed.
// ===============================

void drawNameEntryScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set sky blue background
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Enable blending for text
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw prompt with full opacity
    std::string prompt = "ENTER YOUR NAME";
    float promptScale = 1.2f;
    float promptWidth = prompt.length() * 32 * promptScale * 0.6f;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // Full white with full opacity
    renderText(prompt, WIDTH / 2 - promptWidth / 2, HEIGHT / 2 + 120, promptScale, { 1,1,1 });

    // Name entry text with full opacity
    std::string nameDisplay = playerName;
    float nameScale = 0.8f;
    float nameWidth = nameDisplay.length() * 32 * nameScale * 0.6f;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // Full white with full opacity
    renderText(nameDisplay, WIDTH / 2 - nameWidth / 2, HEIGHT / 2, nameScale, { 1,1,1 });

    // Add instruction for gender guesses
    std::string genderMsg = "ADD <SPACE>M OR <SPACE>F TO POKEMON NAME FOR MALE AND FEMALE GUESSES WHEN NEEDED";
    float genderScale = 0.6f;
    float genderWidth = genderMsg.length() * 32 * genderScale * 0.6f;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    renderText(genderMsg, WIDTH / 2 - genderWidth / 2, HEIGHT / 2 - 80, genderScale, { 1,1,1 });

    // Continue message with full opacity
    std::string continueMsg = "PRESS ENTER TO CONTINUE";
    float continueScale = 0.7f;
    float continueWidth = continueMsg.length() * 32 * continueScale * 0.6f;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // Full white with full opacity
    renderText(continueMsg, WIDTH / 2 - continueWidth / 2, HEIGHT / 2 - 120, continueScale, { 1,1,1 });

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// ===============================
// Function: main
// Purpose: Entry point. Sets up OpenGL, loads assets, initializes game state, and starts the main loop.
// Notes: Change window size, shader paths, and asset paths here. All initialization is done before main loop.
// ===============================

int main(int argc, char** argv) {
    // Initialize game state
    gameState = START_SCREEN;
    playerName.clear();
    currentGuess.clear();
    remainingTime = TIMER_LIMIT;
    flashAlpha = 1.0f;
    flashDir = false;

    // GLUT initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Pok3Dex");
    glutPositionWindow(100, 100);
    glutReshapeWindow(WIDTH, HEIGHT);

    // Critical: Initialize GLEW before any OpenGL operations
    glewInit();

    // Verify OpenGL context
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

    // Set core OpenGL states
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Load font FIRST before any textures/shaders
    std::cout << "Loading font...\n";
    loadFont("assets/fonts/pokemon_gb.ttf");
    if (characters.empty()) {
        std::cerr << "Font load failed!\n";
        return -1;
    }
    glFinish();  // Force GPU sync after font loading

    // Initialize shaders
    shaderProgram = createShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");
    glUseProgram(0);  // Explicitly unbind shader

    // Load textures
    startBg.id = loadTexture("assets/textures/start_bg.png");
    gameBg.id = loadTexture("assets/textures/game_bg.png");
    pokeballTex.id = loadTexture("assets/textures/pokeball.png");

    // Sound initialization
    initSound();

    // GLUT callbacks
    glutDisplayFunc([]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Force fixed-function pipeline for text
        glUseProgram(0);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);

        switch (gameState) {
        case START_SCREEN: drawStartScreen(); break;
        case NAME_ENTRY: drawNameEntryScreen(); break;
        case PLAYING: drawGameScreen(); break;
        case GAME_OVER: drawGameOverScreen(); break;
        case WIN_SCREEN: drawWinScreen(); break;
        }
        glutSwapBuffers();
        });

    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    // Critical: Double-buffered initialization
    glutTimerFunc(100, [](int) {
        glutPostRedisplay();
        std::cout << "Forced first redraw\n";
        }, 0);

    glutMainLoop();
    cleanup();
    return 0;
}
