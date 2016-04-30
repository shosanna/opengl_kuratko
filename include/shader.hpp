#include <glad/glad.h>

class Shader {
 public:
  GLuint program;

  Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
  Shader(const Shader&) = delete;
  ~Shader();

  void use();
};
