
#ifndef __shader_sprite_h
#define __shader_sprite_h

//	Internal shader for sprite

const char *intshd_sprite_vert = {
	"///////////////////////////////////////////////////////////\n"
	"// Attributes\n"
	"attribute vec3 a_position;\n"
	"attribute vec2 a_texCoord;\n"
	"attribute vec4 a_color;\n"
	"\n"
	"///////////////////////////////////////////////////////////\n"
	"// Uniforms\n"
	"uniform mat4 u_projectionMatrix;\n"
	"\n"
	"///////////////////////////////////////////////////////////\n"
	"// Varyings\n"
	"varying vec2 v_texCoord;\n"
	"varying vec4 v_color;\n"
	"\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_Position = u_projectionMatrix * vec4(a_position, 1);\n"
	"    v_texCoord = a_texCoord;\n"
	"    v_color = a_color;\n"
	"}\n"
};

const char *intshd_sprite_frag = {

	"#ifdef OPENGL_ES\n"
	"#extension GL_OES_standard_derivatives : enable\n"
	"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
	"precision highp float;\n"
	"#else\n"
	"precision mediump float;\n"
	"#endif\n"
	"#else\n"
	"precision mediump float;\n"
	"#endif\n"
	"\n"
	"///////////////////////////////////////////////////////////\n"
	"// Uniforms\n"
	"uniform sampler2D u_texture;\n"
	"\n"
	"///////////////////////////////////////////////////////////\n"
	"// Varyings\n"
	"varying vec2 v_texCoord;\n"
	"varying vec4 v_color;\n"
	"\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_FragColor = v_color * texture2D(u_texture, v_texCoord);\n"
	"}\n"
};

const char *intshd_spritecol_vert = {
	"// Attributes\n"
	"attribute vec3 a_position;\n"
	"attribute vec4 a_color;\n"
	"\n"
	"// Uniforms\n"
	"uniform mat4 u_projectionMatrix;\n"
	"\n"
	"// Varyings\n"
	"varying vec4 v_color;\n"
	"\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_Position = u_projectionMatrix * vec4(a_position, 1);\n"
	"    v_color = a_color;\n"
	"}\n"
};

const char *intshd_spritecol_frag = {
	"#ifdef OPENGL_ES\n"
	"#extension GL_OES_standard_derivatives : enable\n"
	"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
	"precision highp float;\n"
	"#else\n"
	"precision mediump float;\n"
	"#endif\n"
	"#else\n"
	"precision mediump float;\n"
	"#endif\n"
	"\n"
	"// Varyings\n"
	"varying vec4 v_color;\n"
	"\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_FragColor = v_color;\n"
	"}\n"
};


const char* intshd_dummy = {
	""
};

#endif

