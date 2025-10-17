#pragma once
#include <string>

class Texture {
public:
  Texture(const std::string &path);
  ~Texture();

  // Rule of 5 for proper resource management
  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;
  Texture(Texture &&other) noexcept;
  Texture &operator=(Texture &&other) noexcept;

  void bind(unsigned int slot = 0) const;
  void unbind() const;

  int get_width() const { return m_width; }
  int get_height() const { return m_height; }
  unsigned int get_id() const { return m_id; }

private:
  unsigned int m_id = 0;
  std::string m_file_path;
  int m_width = 0;
  int m_height = 0;
  int m_channels = 0;
};
