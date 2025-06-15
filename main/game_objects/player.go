components {
  id: "player"
  component: "/main/scripts/player.script"
}
components {
  id: "controls"
  component: "/main/scripts/controls.script"
}
embedded_components {
  id: "player_sprite"
  type: "sprite"
  data: "default_animation: \"idle\"\n"
  "material: \"/builtins/materials/sprite.material\"\n"
  "textures {\n"
  "  sampler: \"texture_sampler\"\n"
  "  texture: \"/assets/atlases/player.atlas\"\n"
  "}\n"
  ""
  scale {
    z: 1.5
  }
}
embedded_components {
  id: "collision"
  type: "collisionobject"
  data: "type: COLLISION_OBJECT_TYPE_DYNAMIC\n"
  "mass: 1.0\n"
  "friction: 0.1\n"
  "restitution: 0.5\n"
  "group: \"default\"\n"
  "mask: \"default\"\n"
  "embedded_collision_shape {\n"
  "  shapes {\n"
  "    shape_type: TYPE_BOX\n"
  "    position {\n"
  "    }\n"
  "    rotation {\n"
  "    }\n"
  "    index: 0\n"
  "    count: 3\n"
  "    id: \"collision\"\n"
  "  }\n"
  "  data: 6.0\n"
  "  data: 7.0\n"
  "  data: 10.0\n"
  "}\n"
  ""
}
