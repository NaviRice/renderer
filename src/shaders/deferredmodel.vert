in vec3 posattrib;
in vec2 tcattrib;
in vec3 normattrib;
//todo depth
//todo norm
uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;

out float depth;
out vec3 pos;
out vec2 tc;
out vec3 norm;
void main(){
	tc = tcattrib;
	vec4 posmodel = vec4(posattrib.xyz, 1.0);
	vec4 worldpos = model * posmodel;
	vec4 viewpos = view * worldpos;
	gl_Position = mvp * worldpos;
//	pos = vec3(model * posmodel);
	depth = viewpos.z;
	pos = worldpos.rgb;
	norm = (model * vec4(normattrib, 0.0)).rgb;
	//todo norm
}
