in vec2 tc;
//in vec2 pos;
in vec3 pos;

uniform sampler2D color;

out vec4 fragData0;
void main(){
	fragData0.rgba = vec4(abs(texture(color, tc).rgb), 1.0);
//	if(fragData0.r > 0.9) fragData0.r -= 0.9;
//	if(fragData0.g > 0.9) fragData0.g -= 0.9;
//	if(fragData0.b > 0.9) fragData0.b -= 0.9;
//	fragData0.rgba = vec4(texture);
}
