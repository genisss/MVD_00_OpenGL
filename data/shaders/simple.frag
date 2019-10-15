#version 330

out vec4 fragColor;
in vec2 v_uv;
uniform sampler2D u_texture_diffuse;
void main(){
	vec3 diffuse_color = texture(u_texture_diffuse, v_uv).xyz;
	fragColor = vec4(diffuse_color, 1);
}