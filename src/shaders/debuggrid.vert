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
	bool ontarget;
} vs_out;
void main(){
	vec4 worldspace;
	vs_out.tc = tcattrib;
	worldspace = vec4(texture(posdata, tcattrib).rgb, 1.0);
	vs_out.ontarget = (worldspace != vec4(0.0, 0.0, 0.0, 1.0));
	vs_out.pos = worldspace.xyz;
	if(bounces == 0){
		worldspace = vstmodel * vec4(posattrib, 0.9, 1.0);
	}

	gl_Position = debugmv * worldspace;
}
