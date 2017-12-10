in vec3 posattrib;
in vec2 tcattrib;

uniform mat4 mvp;

uniform sampler2D offsetmap;
uniform sampler2D normalmap;

out vec3 pos;
out vec2 tc;
void main(){
	pos = posattrib;
	tc = tcattrib;
	pos.z = texture(offsetmap, tc).r;
	gl_Position = mvp * vec4(pos, 1.0);
}
