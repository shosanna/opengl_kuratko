#include <format.h>
#include <cmath>
#include <iostream>

// GLAD
#include <glad/glad.h>

// SDL
#include <SDL/SDL.h>

// TGA Image
#include <tgaimage.h>

// PNG image
#include <lodepng.h>

// IMGUI
#include <imgui.h>
#include <imgui_impl_sdl.h>

#include <stdio.h>
#include <chrono>
#include <fstream>
#include <stopwatch.hpp>
#include <string>
#include <vector>

//#include "tiled.hpp"
#include <shader.hpp>

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

SDL_Window* setupSDL() {
  std::cout << "Starting SDL context, OpenGL 4.1" << std::endl;
  // Init SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // Set all the required options for SDL
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  // Create a GLFWwindow object that we can use for GLFW's functions
  SDL_Window* window = SDL_CreateWindow("Kuratko Nufik", 200, 200, WIDTH,
                                        HEIGHT, SDL_WINDOW_OPENGL);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    return nullptr;
  }

  return window;
}

void tile_at(std::vector<float>& vbo_data, float x, float y, float size) {
  float off = size / 2;

  // Position      Color             Texture coords
  std::vector<float> vertices{
      x + off, y + off, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
      x + off, y - off, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      x - off, y - off, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

      x + off, y + off, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
      x - off, y - off, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
      x - off, y + off, 1.0f, 1.0f, 1.0,  0.0f, 0.0f,
  };

  for (float v : vertices) {
    vbo_data.push_back(v);
  }
}

int current_x = 0;
int current_y = 0;

void draw_vector_triangles(const std::vector<float>& vbo_data) {
  // sending vertices to the graphic cards and making it active
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbo_data.size(),
               vbo_data.data(), GL_STATIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, vbo_data.size());
}

void prepareTex(GLuint texid, std::string filename, int unit) {
  // Nacteni obrazku
  std::vector<unsigned char> background;
  unsigned width, height;
  lodepng::decode(background, width, height, filename);

  // PRVNI TEXTURE
  // Prepnuti se na prvni texturu
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, texid);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, background.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void game_loop(SDL_Window* window) {
  // Setup ImGui binding
  ImGui_ImplSdlGL3_Init(window);

  glViewport(0, 0, WIDTH, HEIGHT);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Inicializovani indexu TEXTUR
  GLuint textures[2];
  glGenTextures(2, textures);

  prepareTex(textures[0], "grass.png", 0);
  prepareTex(textures[1], "kuratko.png", 1);

  Shader shader("vertex.glsl", "fragment.glsl");
  shader.use();

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // Here are 3 attributes - for position, color and texture and a way how to
  // access it from the vertices attributes
  GLint posAttrib = glGetAttribLocation(shader.program, "position");
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
  glEnableVertexAttribArray(posAttrib);

  GLint colAttrib = glGetAttribLocation(shader.program, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void*)(2 * sizeof(float)));

  GLint texAttrib = glGetAttribLocation(shader.program, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void*)(5 * sizeof(float)));

  float tile_size = 0.1f;
  int tile_count = 2 / tile_size;
  std::vector<float> background_data;

  Stopwatch st;

  SDL_Event event;
  GLint u_activeTex = glGetUniformLocation(shader.program, "activeTex");

  if (u_activeTex == -1) {
    std::cerr << "Chyba activeTEx" << std::endl;
  }

  while (true) {
    st.start();
    ImGui_ImplSdlGL3_NewFrame(window);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT ||
          (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)) {
        return;
      }

      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case 'w': current_y++; break;
          case 's': current_y--; break;
          case 'a': current_x--; break;
          case 'd': current_x++; break;
        }
      }

      ImGui_ImplSdlGL3_ProcessEvent(&event);
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform1i(u_activeTex, 0);

    std::vector<float> vbo_data;

    // KURATKO
    {
      float x = current_x * tile_size;
      float y = current_y * tile_size;
      glUniform1i(u_activeTex, 1);
      tile_at(vbo_data, x, y, tile_size);
    }

    draw_vector_triangles(vbo_data);

    ImGui::Begin("he");
    ImGui::Text("Hello");
    ImGui::End();

    ImGui::Render();

    SDL_GL_SwapWindow(window);
  }
}

// The MAIN function, from here we start the application and run the game loop
int main(int, char**) {
  // auto res = load_tiles("xmlova.tmx");

  SDL_Window* window = setupSDL();
  if (!window) return -1;

  SDL_GLContext context = SDL_GL_CreateContext(window);
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return 1;
  }

  game_loop(window);

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

int clamp(int low, int current, int high) {
  if (current < low) return low;
  if (current > high) return high;
  return current;
}
