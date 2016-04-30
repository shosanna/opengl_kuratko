#include "shaderSource.hpp"

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
  }

  ShaderProgram(const ShaderProgram&) = delete;

  ~ShaderProgram() {
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
  }

  void use() { glUseProgram(shaderProgram); }

  void setupAttributes() {
  }
};
