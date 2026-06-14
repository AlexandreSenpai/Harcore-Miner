#include "raylibRenderInterface.h"
#include <raymath.h>
#include <rlgl.h>

struct CustomGeometry {
  std::vector<Rml::Vertex> vertices;
  std::vector<int> indices;
};

Rml::CompiledGeometryHandle
CustomRenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
                                       Rml::Span<const int> indices) {
  CustomGeometry *geometry = new CustomGeometry();
  geometry->vertices.assign(vertices.begin(), vertices.end());
  geometry->indices.assign(indices.begin(), indices.end());
  return (Rml::CompiledGeometryHandle)geometry;
}

void CustomRenderInterface::RenderGeometry(
    Rml::CompiledGeometryHandle geometry_handle, Rml::Vector2f translation,
    Rml::TextureHandle texture) {
  auto geometry = (CustomGeometry *)geometry_handle;
  if (!geometry)
    return;

  if (geometry->indices.empty())
    return;

  // Verify texture handle or fallback to 0 (default untextured rlgl state)
  unsigned int rlTextureId =
      texture ? ((Texture2D *)texture)->id : rlGetTextureIdDefault();

  // Prepare Raylib Graphics rendering state
  rlDrawRenderBatchActive();
  rlDisableBackfaceCulling();

  // IMPORTANT: RmlUi outputs top-left pixel coordinates.
  // We need to ensure raylib rendering uses an orthographic matrix spanning 0
  // to ScreenWidth/Height.
  rlPushMatrix();
  rlLoadIdentity();

  // Apply translation if there is an active global transform
  if (m_hasTransform) {
    // Translation happens before the custom transform usually, depending on
    // RmlUi's expectations. Actually RmlUi expects translation to be applied
    // first, then transform.
    rlTranslatef(translation.x, translation.y, 0.0f);

    Matrix customMat = MatrixTranspose(*(Matrix *)m_transform.data());
    rlMultMatrixf(MatrixToFloat(customMat));
  } else {
    // Just standard translation
    rlTranslatef(translation.x, translation.y, 0.0f);
  }

  rlBegin(RL_TRIANGLES);
  rlSetTexture(rlTextureId);

  // Render triangles manually
  for (size_t i = 0; i < geometry->indices.size(); i++) {
    // Raylib batching limit check
    if (rlCheckRenderBatchLimit(1)) {
      rlBegin(RL_TRIANGLES);
      rlSetTexture(rlTextureId);
    }

    int index = geometry->indices[i];
    const auto &vertex = geometry->vertices[index];

    rlColor4ub(vertex.colour.red, vertex.colour.green, vertex.colour.blue,
               vertex.colour.alpha);
    rlTexCoord2f(vertex.tex_coord.x, vertex.tex_coord.y);
    rlVertex2f(vertex.position.x, vertex.position.y);
  }

  rlEnd();

  // Restore state
  rlPopMatrix();
  rlSetTexture(rlGetTextureIdDefault());
  rlEnableBackfaceCulling();
}

void CustomRenderInterface::ReleaseGeometry(
    Rml::CompiledGeometryHandle geometry_handle) {
  auto geometry = (CustomGeometry *)geometry_handle;
  delete geometry;
}

void CustomRenderInterface::EnableScissorRegion(bool enable) {
  if (enable) {
    // Scissoring will be set by SetScissorRegion
  } else {
    EndScissorMode();
  }
}

void CustomRenderInterface::SetScissorRegion(Rml::Rectanglei region) {
  BeginScissorMode(region.Left(), region.Top(), region.Width(),
                   region.Height());
}

Rml::TextureHandle
CustomRenderInterface::LoadTexture(Rml::Vector2i &texture_dimensions,
                                   const Rml::String &source) {
  // Relying on RmlUi to pass fully qualified absolute paths via the
  // FileInterface parsing.
  if (!FileExists(source.c_str())) {
    return 0; // Invalid texture
  }

  Texture2D rlTexture = ::LoadTexture(source.c_str());
  if (rlTexture.id == 0) {
    return 0;
  }

  auto allocation = new Texture2D(rlTexture);
  texture_dimensions.x = rlTexture.width;
  texture_dimensions.y = rlTexture.height;

  return (Rml::TextureHandle)allocation;
}

Rml::TextureHandle
CustomRenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source,
                                       Rml::Vector2i source_dimensions) {
  Image image = {0};
  image.width = source_dimensions.x;
  image.height = source_dimensions.y;
  image.mipmaps = 1;
  image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
  image.data = (void *)source.data();

  auto texture = LoadTextureFromImage(image);

  if (texture.id == 0) {
    return 0;
  }

  auto allocation = new Texture2D(texture);

  return (Rml::TextureHandle)allocation;
}

void CustomRenderInterface::ReleaseTexture(Rml::TextureHandle texture) {
  if (!texture)
    return;
  auto tex = (Texture2D *)texture;
  UnloadTexture(*tex);
  delete tex;
}

void CustomRenderInterface::SetTransform(const Rml::Matrix4f *transform) {
  if (transform) {
    m_hasTransform = true;
    m_transform = *transform;
  } else {
    m_hasTransform = false;
  }
}