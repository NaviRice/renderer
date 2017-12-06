in vec2 posattrib;
in vec2 tcattrib;

uniform mat4 vstmodel;
uniform mat4 debugmv;
uniform int bounces;// ubershader?

uniform sampler2D posdata;

out VS_OUT {
	vec3 pos;
	vec2 tc;
	vec4 col;
} vs_out;
void main(){
	vec4 worldspace;
	vs_out.tc = tcattrib;
	if(bounces == 0){
		worldspace = vstmodel * vec4(posattrib, 0.9, 1.0);
		vs_out.col = vec4(1.0, 0.2, 0.0, 1.0);
	} else {
		worldspace = vec4(texture(posdata, tcattrib).rgb, 1.0);
		vs_out.col = vec4(0.2, 1.0, 0.0, 1.0);
	}
	vs_out.pos = worldspace.xyz;

	gl_Position = debugmv * worldspace;
}
