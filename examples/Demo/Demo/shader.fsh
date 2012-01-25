#version 150

in vec4 whacky;

in vec4 vertex_color;
out vec4 fragment_colour;

void main(void)
{
    fragment_colour = vertex_color * vec4(whacky.x, 1.0, 1.0, 1.0);
}
