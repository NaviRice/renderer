in vec2 posattrib;
in vec2 tcattrib;

uniform mat4 mvp;
uniform sampler2D posdata;

out VS_OUT {
	vec3 pos;
	vec2 tc;
	vec4 col;
	bool ontarget;
} vs_out;
void main(){
	vs_out.pos = vec3(posattrib, 1.0);
	vs_out.tc = tcattrib;
	vs_out.ontarget = texture(posdata, tcattrib).rgb != vec3(0.0, 0.0, 0.0);
	gl_Position = mvp * vec4(vs_out.pos, 1.0);
}
