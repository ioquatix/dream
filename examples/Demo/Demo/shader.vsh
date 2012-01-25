#version 150

uniform vec2 p;

uniform mat4 display_matrix;

in vec4 position;
in vec4 colour;

out vec4 vertex_color;
out vec4 whacky;

void main (void)
{
    vertex_color = colour;
	
	whacky = vec4(sin(p.x * 2), sin(p.y * 2), 0.0, 0.0) + position;
    gl_Position = display_matrix * (vec4(p, 0.0, 0.0) + position);
}
