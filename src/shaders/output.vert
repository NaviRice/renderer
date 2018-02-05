in vec2 posattrib;
in vec2 tcattrib;

uniform mat4 vstmodel;


uniform mat4 invmodelscreen;
uniform mat4 modelscreen;


uniform vec3 raystart; //campos

uniform sampler2D posdata;
uniform sampler2D normdata;

out vec2 tc;
void main(){
	vec4 worldspace;
	tc = tcattrib;
	worldspace = vec4(texture(posdata, tcattrib).rgb, 1.0);
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
//		screenspace.xy = clamp(screenspace.xy, vec2(-1.0), vec2(1.0));

		//now transform back into worldspace
		gl_Position = vec4(screenspace, 1.0);
}
