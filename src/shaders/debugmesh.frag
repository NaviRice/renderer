in vec2 tc;
in vec3 pos;

uniform sampler2D depth;

out vec4 fragData0;
void main(){
	if(abs(mod(tc.x*20.0, 1) - 0.5) < 0.4 &&abs(mod(tc.y*10.0, 1) - 0.5) < 0.4) discard;
	fragData0 = vec4(0.0, pos.z, 1.0, 1.0);
}
