/**
 * This file is a helper to compensate for insufficient data during development
 * and will eventually be removed.
 */

#pragma once

#include "core/logger.h"

namespace zen::remote::client {

namespace {

const char *default_vertex_shader =
    "#version 320 es\n"
    "uniform mat4 mvp;\n"
    "layout(location = 0) in vec4 position;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = mvp * position;\n"
    "}\n";

const char *default_color_fragment_shader =
    "#version 320 es\n"
    "precision mediump float;\n"
    "out vec4 outputColor;\n"
    "void main()\n"
    "{\n"
    "  outputColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

}  // namespace

class TmpRenderingHelper {
 public:
  static GLuint CompileShader(const char *shader, uint64_t type)
  {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &shader, NULL);
    glCompileShader(id);

    GLint compiled = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
      int log_length = 0;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_length);
      std::string log_message(log_length, ' ');
      glGetShaderInfoLog(id, log_length, NULL, log_message.data());
      LOG_ERROR("Compiling %s shader: %s",
          type == GL_VERTEX_SHADER     ? "vertex"
          : type == GL_FRAGMENT_SHADER ? "fragment"
                                       : "",
          log_message.c_str());
      glDeleteShader(id);
      return 0;
    }

    return id;
  };

  static GLuint CompilePrograms(
      const char *vertex_shader, const char *fragment_shader)
  {
    GLuint program_id = glCreateProgram();

    GLuint vertex_shader_id = CompileShader(vertex_shader, GL_VERTEX_SHADER);
    if (vertex_shader_id == 0) {
      glDeleteProgram(program_id);
      return 0;
    }
    glAttachShader(program_id, vertex_shader_id);
    glDeleteShader(vertex_shader_id);

    GLuint fragment_shader_id =
        CompileShader(fragment_shader, GL_FRAGMENT_SHADER);
    if (fragment_shader_id == 0) {
      glDeleteProgram(program_id);
      return 0;
    }
    glAttachShader(program_id, fragment_shader_id);
    glDeleteShader(fragment_shader_id);

    glLinkProgram(program_id);

    GLint linked = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
      int log_length = 0;
      glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
      std::string log_message(log_length, ' ');
      glGetProgramInfoLog(program_id, log_length, NULL, log_message.data());
      LOG_ERROR("Linking shaders: %s", log_message.c_str());
      glDeleteProgram(program_id);
      return 0;
    }

    return program_id;
  };

  GLuint program_id_;
};

}  // namespace zen::remote::client
