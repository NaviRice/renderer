layout (triangles) in;
layout (line_strip, max_vertices=4) out;


in VS_OUT {
	vec3 pos;
	vec2 tc;
	vec4 col;
	bool ontarget;
} gs_in[];

out VS_OUT {
	vec3 pos;
	vec2 tc;
	vec4 col;
} gs_out;

void main(){


	vec4 col;
	bool anytarget = gs_in[0].ontarget && gs_in[1].ontarget && gs_in[2].ontarget;
	if(anytarget) col = vec4(0.0, 1.0, 0.0, 1.0);
	else col = vec4(1.0, 0.0, 0.0, 1.0);

	gl_Position = gl_in[0].gl_Position;
	gs_out.col = col;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	gs_out.col = col;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	gs_out.col = col;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position;
	gs_out.col = col;
	EmitVertex();

	EndPrimitive();
}
