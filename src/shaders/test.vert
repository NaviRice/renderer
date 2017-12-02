

in vec3 posattrib;
in vec3 normattrib;
in vec2 tcattrib;

uniform mat4 mvp;

out vec3 pos;
out vec3 norm;
out vec2 tc;
void main(){
	gl_Position = mvp * vec4(posattrib, 1.0);
	pos = posattrib;
	norm = (mvp * vec4(normattrib, 0.0)).xyz;
	tc = tcattrib;
}
