

in VS_OUT {
	vec3 pos;
	vec2 tc;
	vec4 col;
} fs_in;

uniform sampler2D depth;

out vec4 fragData0;
void main(){
//	if(abs(mod(tc.x*20.0, 1) - 0.5) < 0.4 &&abs(mod(tc.y*10.0, 1) - 0.5) < 0.4) discard;
	fragData0 = fs_in.col;
}
