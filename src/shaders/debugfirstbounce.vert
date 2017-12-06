in vec2 posattrib;
in vec2 tcattrib;

uniform sampler2D color;

//uniform vec4 resize;
uniform mat4 mvp;

//out vec2 pos;
out vec3 pos;
out vec2 tc;
void main(){
//	pos = posattrib*resize.xy + resize.zw; //simple hack because i dont feel like using matricies or stuff
	pos = vec3(posattrib, 1.0);
	tc = tcattrib;
//	gl_Position = vec4(pos, 1.0, 1.0);
	gl_Position = mvp * vec4(pos, 1.0);
}
