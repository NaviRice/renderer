in vec3 posattrib;
in vec2 tcattrib;
//todo depth
//todo norm
uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;

uniform sampler2D offsetmap;
uniform sampler2D normamap; // modelspace, i do this in frag shader

out float depth;
out vec3 pos;
out vec2 tc;
void main(){
	tc = tcattrib;
	vec4 posmodel = vec4(posattrib.xy, texture(offsetmap, tcattrib).r, 1.0);
	vec4 worldpos = model * posmodel;
	vec4 viewpos = view * worldpos;
	gl_Position = mvp * worldpos;
//	pos = vec3(model * posmodel);
	depth = viewpos.z;
	pos = worldpos.rgb;
	//todo norm
}
