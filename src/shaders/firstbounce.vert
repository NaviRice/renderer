in vec3 posattrib;
in vec2 tcattrib;
//todo depth
//todo norm
uniform mat4 mvp;
uniform mat4 model;

uniform sampler2D depth;

out vec3 pos;
out vec2 tc;
void main(){
	tc = tcattrib;
	vec4 posmodel = vec4(posattrib.xy, texture(depth, tc).r, 1.0);
	vec4 worldpos = model * posmodel;
	gl_Position = mvp * worldpos;
	pos = vec3(model * posmodel);
}
