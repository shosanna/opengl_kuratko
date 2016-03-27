#include <iostream>
#include <cmath>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// TGA Image
#include <tgaimage.h>

#include <string>
#include <stdio.h>
#include <fstream>
#include <chrono>
#include <vector>

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

// This example is taken from http://learnopengl.com/
// http://learnopengl.com/code_viewer.php?code=getting-started/hellowindow2
// The code originally used GLEW, I replaced it with Glad

// Compile:
// g++ example/c++/hellowindow2.cpp -Ibuild/include build/src/glad.c -lglfw -ldl

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

void error_callback(int error, const char* description) {
  std::cout << description << std::endl;
}

GLFWwindow* setupGLFW() {
  std::cout << "Starting GLFW context, OpenGL 3.2" << std::endl;
  // Init GLFW
  glfwInit();
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwSetErrorCallback(error_callback);

  // Create a GLFWwindow object that we can use for GLFW's functions
  GLFWwindow* window =
      glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, key_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return nullptr;
  }

  return window;
}

void kocicka_at(float x, float y, float size) {
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);

  float  off = size / 2;
  // Position      Color             Texture coords
  float vertices[] = {
    x + off, y + off, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    x + off, y - off, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 
    x - off, y - off, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

    x + off, y + off, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    x - off, y - off, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    x - off, y + off, 1.0f, 1.0f, 1.0, 0.0f, 0.0f,
  };

  // sending vertices to the graphic cards and making it active
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Vao for caching attributes!
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Compiling shaders
  ShaderSource vs("vertex.glsl");
  GLuint vertexShader = vs.compile(GL_VERTEX_SHADER);

  ShaderSource fs("fragment.glsl");
  GLuint fragmentShader = fs.compile(GL_FRAGMENT_SHADER);

  // Combining all shaders together
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Here are 3 attributes - for position, color and texture and a way how to access it from the vertices attributes
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
  glEnableVertexAttribArray(posAttrib);

  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));

  GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));

  // UNIFORM = global variable to set a color of a shape globally
  GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
  glUniform3f(uniColor, 1.5f, 0.0f, 0.0f);

  // Inicializovani indexu TEXTUR
  GLuint textures[2];
  glGenTextures(2, textures);
  glUniform1i(glGetUniformLocation(shaderProgram, "kocicka"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "pejsek"), 1);

  // Prepnuti se na prvni texturu
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);

  // PRVNI TEXTURE
  TGAImage kocicka;
  if (!kocicka.read_tga_file("sample.tga")) {
    std::cerr << "Nepovedlo se" << std::endl;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kocicka.get_width(), kocicka.get_height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, kocicka.buffer());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // DRUHA TEXTURA
  
  // Prepnuti se na prvni texturu
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures[1]);

  TGAImage pejsek;
  if (!pejsek.read_tga_file("pejsek.tga")) {
    std::cerr << "Nepovedlo se" << std::endl;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pejsek.get_width(), pejsek.get_height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, pejsek.buffer());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void game_loop(GLFWwindow* window) {
  glViewport(0, 0, 2*WIDTH, 2*HEIGHT);


  // auto t_start = std::chrono::high_resolution_clock::now();
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Changing global color (Uniform) over time
    // auto t_now = std::chrono::high_resolution_clock::now();
    // float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
    // glUniform3f(uniColor, (std::sin(time*4.0f) + 1.0f) / 2.0f, 0.3f, 0.87f);
    kocicka_at(0,0, 0.2);
    glfwSwapBuffers(window);
  }
}

// The MAIN function, from here we start the application and run the game loop
int main() {
  GLFWwindow* window = setupGLFW();
  if (!window) return -1;

  game_loop(window);

  // Terminates GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();
  return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode) {
  std::cout << key << std::endl;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}
