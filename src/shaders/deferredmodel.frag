
out vec4 fragData0; //pos in worldspace, depth in viewspace
out vec4 fragData1; //norm in worldspace



uniform sampler2D normalmap;

uniform mat4 model;


in float depth;
in vec3 pos;
in vec3 norm;
in vec2 tc;
void main(){
//	if(abs(mod(tc.x*20.0, 1) - 0.5) < 0.4 &&abs(mod(tc.y*10.0, 1) - 0.5) < 0.4) discard;
//	fragData0 = vec4(0.0, pos.z, 1.0, 1.0);
//	fragData0.rgb = pos;
	fragData0.rgb = pos;
	fragData0.a = depth; //temp lol
//	fragData1.rgba = vec4(1.0);
//	fragData1.rgb = normalize(texture(normalmap, tc).rgb);
//	fragData1.rgb = normalize(vec3(model * vec4(texture(normalmap, tc).rgb, 1.0)));
	//fuck it ill dfdx it
//	vec3 p1 = normalize(dFdx(pos));
//	vec3 p2 = normalize(dFdy(pos));
//	fragData1.rgb = normalize(cross(p1, p2));
	fragData1.rgb = normalize(norm);
}
