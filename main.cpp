#include <cmath>
#include <iostream>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// TGA Image
#include <tgaimage.h>

#include <stdio.h>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>

#include "tiled.hpp"

class ShaderSource {
  std::string filename_;
  std::string contents_;
  const char* c_str_;
  const GLchar** source_;

 public:
  ShaderSource(std::string filename) : filename_(filename) {
    std::ifstream is{filename};

    contents_ = {std::istreambuf_iterator<char>(is),
                 std::istreambuf_iterator<char>()};

    c_str_ = contents_.c_str();
    source_ = &c_str_;
  }

  const GLchar** source() { return source_; }

  GLuint compile(GLenum type) {
    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, source(), nullptr);
    glCompileShader(shaderID);

    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
      std::cerr << "Shader from " << filename_ << " failed to compile"
                << std::endl;

      char buffer[512];
      glGetShaderInfoLog(shaderID, 512, nullptr, buffer);
      std::cerr << buffer << std::endl;
    }

    return shaderID;
  }
};

class ShaderProgram {
 public:
  ShaderSource vertexShaderSource_;
  ShaderSource fragmentShaderSource_;

  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint shaderProgram;

  ShaderProgram(std::string vertex_file, std::string fragment_file_)
      : vertexShaderSource_{vertex_file},
        fragmentShaderSource_{fragment_file_} {
    vertexShader = vertexShaderSource_.compile(GL_VERTEX_SHADER);
    fragmentShader = fragmentShaderSource_.compile(GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    // Vao for caching attributes!
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
  }

  ShaderProgram(const ShaderProgram&) = delete;

  ~ShaderProgram() {
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
  }

  void use() { glUseProgram(shaderProgram); }

  void setupAttributes() {
    glBindFragDataLocation(shaderProgram, 0, "outColor");

    int vert_data_size = 5 * sizeof(float);

    // Here are 3 attributes - for position, color and texture and a way how to
    // access it from the vertices attributes
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, vert_data_size, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, vert_data_size,
                          (void*)(2 * sizeof(float)));
  }
};

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

SDL_Window* setupSDL() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_Window* window =
      SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
  if (window == nullptr) {
    std::cout << "Failed to create SDL window" << std::endl;
    SDL_Quit();
    return nullptr;
  }

  SDL_GLContext context = SDL_GL_CreateContext(window);
  if (context == nullptr) {
    std::cout << "Failed to create SDL context" << std::endl;
    SDL_Quit();
    return nullptr;
  }

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return 0;
  }

  return window;
}

void tile_at(float x, float y, float size, float r, float g, float b) {

  float off = size / 2;
  // Position      Color             Texture coords
  float vertices[] = {
      x + off, y + off, r, g, b,
      x + off, y - off, r, g, b,
      x - off, y - off, r, g, b,

      x + off, y + off, r, g, b,
      x - off, y - off, r, g, b,
      x - off, y + off, r, g, b,
  };
  // sending vertices to the graphic cards and making it active
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

int current_x = 0;
int current_y = 0;

void game_loop(SDL_Window* window) {
  glViewport(0, 0, 2 * WIDTH, 2 * HEIGHT);

  // Nacteni obrazku
  TGAImage background;
  if (!background.read_tga_file("grass.tga")) {
    std::cerr << "Nepovedlo se" << std::endl;
  }
  TGAImage kuratko;
  if (!kuratko.read_tga_file("kuratko.tga")) {
    std::cerr << "Nepovedlo se" << std::endl;
  }

  // Inicializovani indexu TEXTUR
  GLuint textures[2];
  glGenTextures(2, textures);

  ShaderProgram program("vertex.glsl", "fragment.glsl");

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  program.use();
  program.setupAttributes();

  SDL_Event windowEvent;
  while (true) {
    if (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT) break;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float tile_size = 0.2f;
    int tile_count = 2 / tile_size;


    for (int i = 0; i < tile_count; i++) {
      for (int j = 0; j < tile_count; j++) {
        float x = j * tile_size - 1 + tile_size / 2;
        float y = -(i * tile_size - 1 + tile_size / 2);

        if (i == current_y && j == current_x) {
          tile_at(x, y, tile_size, 1.0f, 0.0f, 0.0f);
        } else {
          tile_at(x, y, tile_size, 0.0f, 1.0f, 0.0f);
        };
      }
    }
    SDL_GL_SwapWindow(window);
  }
}

// The MAIN function, from here we start the application and run the game loop
int main() {
  auto res = load_tiles("xmlova.tmx");

  // GLFWwindow* window = setupGLFW();
  SDL_Window* window = setupSDL();
  if (!window) return -1;

  game_loop(window);

  // Terminates SDL, clearing any resources allocated by SDL
  // TODO clean up context!!!
  // SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}

int clamp(int low, int current, int high) {
  if (current < low) return low;
  if (current > high) return high;
  return current;
}

// Is called whenever a key is pressed/released via GLFW
// void key_callback(GLFWwindow* window, int key, int scancode, int action,
//                   int mode) {
//   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//     glfwSetWindowShouldClose(window, GL_TRUE);
//
//   if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
//     current_x++;
//   }
//
//   if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
//     current_x--;
//   }
//
//   if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
//     current_y--;
//   }
//
//   if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
//     current_y++;
//   }
//
//   current_y = clamp(0, current_y, 19);
//   current_x = clamp(0, current_x, 19);
// }
