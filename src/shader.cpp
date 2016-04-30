#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include <shader.hpp>

GLuint load_and_compile_shader_(const GLchar* path, GLenum shaderType) {
  using namespace std;

  ifstream file(path);

  stringstream str;
  str << file.rdbuf();

  string code = str.str();
  cout << code << endl << endl;
  const GLchar* code_c = code.c_str();

  GLint success;
  GLchar errorLog[512];

  GLuint shaderId = glCreateShader(shaderType);
  glShaderSource(shaderId, 1, &code_c, nullptr);
  glCompileShader(shaderId);

  // Check for compile errors
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderId, 512, nullptr, errorLog);
    cerr << "ERROR: shader " << path << " failed to compile: " << errorLog << endl;
    throw "Shader failed to compile";
  }

  return shaderId;
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
  using namespace std;

  GLuint vertex = load_and_compile_shader_(vertexPath, GL_VERTEX_SHADER);
  GLuint fragment = load_and_compile_shader_(fragmentPath, GL_FRAGMENT_SHADER);

  program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  GLint success;
  GLchar errorLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, errorLog);
    cerr << "ERROR: program link fail: " << errorLog << endl;
    throw "glLinkProgram failed";
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

Shader::~Shader() { glDeleteProgram(program); }

void Shader::use() { glUseProgram(program); }
