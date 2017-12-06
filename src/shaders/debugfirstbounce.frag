in vec2 tc;
in vec2 pos;

uniform sampler2D color;

out vec4 fragData0;
void main(){
	fragData0.rgba = vec4(abs(texture(color, tc).rgb), 1.0);
//	fragData0.rgba = vec4(texture);
}
