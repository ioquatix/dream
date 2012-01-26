#version 150

uniform vec2 point;

uniform mat4 display_matrix;

uniform vec4 light_positions[2];

in vec4 position;
in vec4 normal;
in vec4 color;
in vec2 mapping;

out vec4 vertex_color;
out vec4 lighting_normal;
out vec4 surface_position;

smooth out vec2 diffuse_mapping;

void main (void)
{
    vertex_color = color;
	lighting_normal = normal;
	diffuse_mapping = mapping;

	surface_position = (vec4(point, 0.0, 0.0) + position);	
    gl_Position = display_matrix * surface_position;
}
