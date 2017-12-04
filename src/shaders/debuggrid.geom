layout (triangles) in;
layout (line_strip, max_vertices=4) out;


in VS_OUT {
	vec3 pos;
	vec2 tc;
	vec4 col;
} gs_in[];

out VS_OUT {
	vec3 pos;
	vec2 tc;
	vec4 col;
} gs_out;

void main(){

	gl_Position = gl_in[0].gl_Position;
	gs_out.col = gs_in[0].col; //tried setting the entire block at once, didnt work
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	gs_out.col = gs_in[1].col;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	gs_out.col = gs_in[2].col;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position;
	gs_out.col = gs_in[0].col;
	EmitVertex();

	EndPrimitive();
}
