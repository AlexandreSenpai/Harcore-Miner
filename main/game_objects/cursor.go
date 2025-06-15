components {
  id: "cursor"
  component: "/main/scripts/cursor.script"
}
components {
  id: "pointer_lock"
  component: "/pointer_lock/pointer_lock.script"
}
embedded_components {
  id: "sprite"
  type: "sprite"
  data: "default_animation: \"cursor\"\n"
  "material: \"/builtins/materials/sprite.material\"\n"
  "textures {\n"
  "  sampler: \"texture_sampler\"\n"
  "  texture: \"/assets/atlases/main.atlas\"\n"
  "}\n"
  ""
  position {
    z: 1.0
  }
  scale {
    x: 0.2
    y: 0.2
  }
}
