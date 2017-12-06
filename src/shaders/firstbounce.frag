in vec3 pos;
in vec2 tc;

uniform sampler2D depth;

out vec4 fragData0; //pos in worldspace (worldstar)
out vec4 fragData1; //norm in worldspace (worldstar) (todo)
void main(){
//	if(abs(mod(tc.x*20.0, 1) - 0.5) < 0.4 &&abs(mod(tc.y*10.0, 1) - 0.5) < 0.4) discard;
//	fragData0 = vec4(0.0, pos.z, 1.0, 1.0);
	fragData0.rgb = pos;
//	fragData0.rgba = vec4(1.0);
//	fragData1.rgb = norm;
}
