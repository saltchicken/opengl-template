#include "graphics/Texture.h"
#include <glad/glad.h>
#include <iostream>
#include <utility>

// This tells stb_image.h to include the implementation.
// It should only be defined in one .cpp file.
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Texture::Texture(const std::string &path) : m_file_path(path) {
  stbi_set_flip_vertically_on_load(true);

  unsigned char *data =
      stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
  if (data) {
    GLenum internal_format = 0;
    GLenum data_format = 0;
    if (m_channels == 4) {
      internal_format = GL_RGBA8;
      data_format = GL_RGBA;
    } else if (m_channels == 3) {
      internal_format = GL_RGB8;
      data_format = GL_RGB;
    }

    if (internal_format == 0 || data_format == 0) {
      std::cerr << "Error: unsupported image format for " << path << " with "
                << m_channels << " channels" << std::endl;
      stbi_image_free(data);
      return;
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_width, m_height, 0,
                 data_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
  } else {
    std::cerr << "Failed to load texture: " << stbi_failure_reason() << " ("
              << path << ")" << std::endl;
  }
}

Texture::~Texture() {
  if (m_id != 0) {
    glDeleteTextures(1, &m_id);
  }
}

Texture::Texture(Texture &&other) noexcept
    : m_id(other.m_id), m_file_path(std::move(other.m_file_path)),
      m_width(other.m_width), m_height(other.m_height),
      m_channels(other.m_channels) {
  other.m_id = 0;
}

Texture &Texture::operator=(Texture &&other) noexcept {
  if (this != &other) {
    if (m_id != 0) {
      glDeleteTextures(1, &m_id);
    }
    m_id = other.m_id;
    m_file_path = std::move(other.m_file_path);
    m_width = other.m_width;
    m_height = other.m_height;
    m_channels = other.m_channels;
    other.m_id = 0;
  }
  return *this;
}

void Texture::bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
