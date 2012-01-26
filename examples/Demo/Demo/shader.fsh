#version 150

uniform vec4 light_positions[2];
uniform sampler2D diffuse;

in vec4 vertex_color;
in vec4 lighting_normal;
in vec4 surface_position;

smooth in vec2 diffuse_mapping;

out vec4 fragment_color;

void main(void)
{
	vec4 light_directions[2];
	float light_distance[2], intensity[2], light[2];
	
	light_directions[0] = light_positions[0] - surface_position;
	light_directions[1] = light_positions[1] - surface_position;
	
	light_distance[0] = length(light_directions[0]);
	light_distance[1] = length(light_directions[1]);
	
	intensity[0] = max(dot(normalize(light_directions[0]), normalize(lighting_normal)), 0.0);
	intensity[1] = max(dot(normalize(light_directions[1]), normalize(lighting_normal)), 0.0);
	
	light[0] = intensity[0] * (2.0 / (light_distance[0] + 1.0));
	light[1] = intensity[1] * (2.0 / (light_distance[1] + 1.0));
	
	//vec4 mapping_color = vec4(diffuse_mapping.s, diffuse_mapping.t, 0.0, 1.0);
	fragment_color = texture(diffuse, diffuse_mapping);
	
    //fragment_color = vec4(0.1) + ((light[0] * vec4(1.0, 0.0, 0.0, 1.0)) + (light[1] * vec4(0.0, 1.0, 0.0, 1.0)));
	fragment_color.w = 1.0;
}
