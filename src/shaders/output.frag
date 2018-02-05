uniform sampler2D color;

in vec2 tc;
out vec4 fragData0;
void main(){
//	if(abs(mod(tc.x*20.0, 1) - 0.5) < 0.4 &&abs(mod(tc.y*10.0, 1) - 0.5) < 0.4) discard;
	fragData0 = texture(color, tc);
//	fragData0 += vec4(tc.x, tc.y, 0.0, 1.0);
}
