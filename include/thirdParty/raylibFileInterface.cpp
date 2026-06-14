#include "raylibFileInterface.h"
#include <RmlUi/Core/Log.h>
#include <filesystem>
#include <fstream>


Rml::FileHandle CustomFileInterface::Open(const Rml::String &path) {
  std::filesystem::path p(path);
  std::string finalPath = p.string();

  // If it's a relative path, assume it's relative to the project root (or
  // wherever the executable runs) Raylib's GetWorkingDirectory() or simple
  // relative assumes the CWD. We'll rely on the standard `../` parsing if
  // needed, but simple `assets/` relative works best if we're running from
  // `build/`. Let's resolve against the current working directory cleanly:
  if (!p.is_absolute()) {
    // Since we run in build/, "assets/ui/hello.rml" actually means
    // "../assets/ui/hello.rml" We'll let the user provide paths like
    // "../assets/" explicitly or dynamically parse. Let's just try to open it
    // exactly as supplied first, and if it fails, try prepending "../" assuming
    // running from build.
    std::filesystem::path attempt1 = p;
    if (!std::filesystem::exists(attempt1)) {
      std::filesystem::path attempt2 =
          std::filesystem::current_path() / ".." / p;
      if (std::filesystem::exists(attempt2)) {
        finalPath = attempt2.string();
      }
    }
  }

  auto stream = new std::fstream(finalPath, std::ios::in | std::ios::binary);

  if (!stream->is_open()) {
    delete stream;
    Rml::Log::Message(Rml::Log::LT_WARNING,
                      "CustomFileInterface: Failed to open file: %s",
                      finalPath.c_str());
    return 0;
  }

  return (Rml::FileHandle)stream;
}

void CustomFileInterface::Close(Rml::FileHandle file) {
  if (!file)
    return;
  auto stream = (std::fstream *)file;
  stream->close();
  delete stream;
}

size_t CustomFileInterface::Read(void *buffer, size_t size,
                                 Rml::FileHandle file) {
  if (!file)
    return 0;
  auto stream = (std::fstream *)file;
  stream->read(reinterpret_cast<char *>(buffer), size);
  return stream->gcount();
}

bool CustomFileInterface::Seek(Rml::FileHandle file, long offset, int origin) {
  if (!file)
    return false;
  auto stream = (std::fstream *)file;

  std::ios::seekdir direction = std::ios::beg;
  switch (origin) {
  case SEEK_CUR:
    direction = std::ios::cur;
    break;
  case SEEK_END:
    direction = std::ios::end;
    break;
  case SEEK_SET:
    direction = std::ios::beg;
    break;
  default:
    break;
  }

  stream->seekg(offset, direction);
  return !stream->fail();
}

size_t CustomFileInterface::Tell(Rml::FileHandle file) {
  if (!file)
    return 0;
  auto stream = (std::fstream *)file;
  return stream->tellg();
}