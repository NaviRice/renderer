in vec2 tc;
in vec3 pos;

uniform sampler2D depth;

out vec4 fragData0;
void main(){
//	if(abs(mod(tc.x*100.0, 1) - 0.5) > 0.1) discard;
//	if(abs(mod(tc.y*100.0, 1) - 0.5) > 0.1) discard;
	fragData0 = vec4(0.0, pos.z, 1.0, 1.0);
}