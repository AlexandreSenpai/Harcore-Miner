#include <map>
#include <raylib.h>
#include <string>

class AssetManager {
private:
  static AssetManager *instance;
  std::map<std::string, Texture2D> textures;
  std::map<std::string, Shader> shaders;

  AssetManager();
  ~AssetManager();

public:
  static AssetManager *GetInstance();

  Texture2D LoadTexture(const char *path);
  void UnloadTexture(const char *path);

  Texture2D *GetTexture(const char *path);

  Shader LoadShader(const char *path);
  void UnloadShader(const char *path);

  Shader *GetShader(const char *path);
};