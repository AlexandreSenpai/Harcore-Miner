#include "raylibFileInterface.h"
#include "engine/core/asset_manager.h"
#include <RmlUi/Core/Log.h>
#include <filesystem>
#include <fstream>


Rml::FileHandle CustomFileInterface::Open(const Rml::String &path) {
  std::string finalPath = AssetManager::GetResourcePath(path);

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