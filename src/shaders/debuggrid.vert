in vec2 posattrib;
in vec2 tcattrib;

uniform mat4 mvp;

out VS_OUT {
	vec3 pos;
	vec2 tc;
	vec4 col;
} vs_out;
void main(){
	vs_out.pos = vec3(posattrib, 0.95);
	vs_out.tc = tcattrib;
	vs_out.col = vec4(1.0, 0.2, 0.0, 1.0);
	gl_Position = mvp * vec4(vs_out.pos, 1.0);
}
