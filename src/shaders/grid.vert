in vec2 posattrib;
in vec2 tcattrib;

uniform mat4 vstmodel;


uniform mat4 invmodelscreen;
uniform mat4 modelscreen;
uniform int bounces;// ubershader?


uniform vec3 raystart; //campos

uniform sampler2D posdata;
uniform sampler2D normdata;

out VS_OUT {
	vec3 pos;
	vec2 tc;
	vec4 col;
	bool ontarget;
} vs_out;
void main(){
	vec4 worldspace;
	vs_out.tc = tcattrib;
	worldspace = vec4(texture(posdata, tcattrib).rgb, 1.0);
	vs_out.ontarget = (worldspace != vec4(0.0, 0.0, 0.0, 1.0));
	vs_out.pos = worldspace.xyz;
//	if(bounces == 0){
//		worldspace = vstmodel * vec4(posattrib, 0.9, 1.0);
//	} else if(bounces == 2){
		//get initial ray direction vector
		vec3 p0 = normalize(worldspace.xyz - raystart);
		vec3 norm = normalize(texture(normdata, tcattrib).rgb);
		//fugggggg
		vec3 outdir = reflect(p0, norm);

//		worldspace.xyz += outdir * 2.0;
		//transform worldspace into "screen view space"
		vec3 screenspace = vec3(invmodelscreen * vec4(worldspace.xyz, 1.0));
		vec3 screennorm = vec3(invmodelscreen * vec4(outdir, 0.0));
		float dist = screenspace.z / screennorm.z;
		screenspace.xy += dist * screennorm.xy;
		screenspace.z = 0.0;

//eh probably should do something different
		screenspace.xy = clamp(screenspace.xy, vec2(-1.0), vec2(1.0));

		//now transform back into worldspace
//		worldspace = modelscreen * vec4(screenspace, 1.0);
		gl_Position = vec4(screenspace, 1.0);
//		vs_out.ontarget &= worldspace

		//ok so we have a worldspace pos, a worldspace normal
//	}

//	gl_Position = debugmv * worldspace;
}
