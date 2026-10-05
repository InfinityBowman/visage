$input a_position, a_color0, a_color1, a_color2, a_texcoord0, a_texcoord1
$output v_coordinates, v_position, v_gradient_pos, v_gradient_pos2 v_gradient_texture_pos

#include <shader_include.sh>

uniform vec4 u_bounds;
uniform vec4 u_atlas_scale;

// An image's quad, clamped: what is cut off a side moves that side's texels by as many as the
// cut covers, a_texcoord0.zw texels to a pixel, so a scaled image is cut and not squeezed.
void main() {
  vec2 min = a_texcoord1.xy;
  vec2 max = a_texcoord1.zw;
  vec2 clamped = clamp(a_position.xy, min, max);
  vec2 delta = clamped - a_position.xy;

  v_position = clamped;
  v_gradient_texture_pos = a_color0;
  v_gradient_pos = a_color1;
  v_gradient_pos2 = a_color2;
  v_coordinates = (a_texcoord0.xy + delta * a_texcoord0.zw) * u_atlas_scale.xy;
  vec2 adjusted_position = clamped * u_bounds.xy + u_bounds.zw;
  gl_Position = vec4(adjusted_position, 0.5, 1.0);
}
