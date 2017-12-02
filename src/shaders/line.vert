in vec3 posattrib;
in vec3 colattrib;

uniform mat4 mvp;

out vec3 col;
void main(){
	gl_Position = mvp * vec4(posattrib, 1.0);
	col = colattrib;
}
