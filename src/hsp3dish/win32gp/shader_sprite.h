
#ifndef __shader_sprite_h
#define __shader_sprite_h

//	Internal shader for sprite

const char *intshd_sprite_vert = {
	"///////////////////////////////////////////////////////////\r\n"
	"// Attributes\r\n"
	"attribute vec3 a_position;\r\n"
	"attribute vec2 a_texCoord;\r\n"
	"attribute vec4 a_color;\r\n"
	"\r\n"
	"///////////////////////////////////////////////////////////\r\n"
	"// Uniforms\r\n"
	"uniform mat4 u_projectionMatrix;\r\n"
	"\r\n"
	"///////////////////////////////////////////////////////////\r\n"
	"// Varyings\r\n"
	"varying vec2 v_texCoord;\r\n"
	"varying vec4 v_color;\r\n"
	"\r\n"
	"\r\n"
	"void main()\r\n"
	"{\r\n"
	"    gl_Position = u_projectionMatrix * vec4(a_position, 1);\r\n"
	"    v_texCoord = a_texCoord;\r\n"
	"    v_color = a_color;\r\n"
	"}\r\n"
};

const char *intshd_sprite_frag = {
	"#if defined(OPENGL_ES) || defined(GL_ES)\r\n"
	"#extension GL_OES_standard_derivatives : enable\r\n"
	"#ifdef GL_FRAGMENT_PRECISION_HIGH\r\n"
	"precision highp float;\r\n"
	"#else\r\n"
	"precision mediump float;\r\n"
	"#endif\r\n"
	"#endif\r\n"
	"\r\n"
	"///////////////////////////////////////////////////////////\r\n"
	"// Uniforms\r\n"
	"uniform sampler2D u_texture;\r\n"
	"\r\n"
	"///////////////////////////////////////////////////////////\r\n"
	"// Varyings\r\n"
	"varying vec2 v_texCoord;\r\n"
	"varying vec4 v_color;\r\n"
	"\r\n"
	"\r\n"
	"void main()\r\n"
	"{\r\n"
	"    gl_FragColor = v_color * texture2D(u_texture, v_texCoord);\r\n"
	"}\r\n"
};

const char *intshd_spritecol_vert = {
	"// Attributes\r\n"
	"attribute vec3 a_position;\r\n"
	"attribute vec4 a_color;\r\n"
	"\r\n"
	"// Uniforms\r\n"
	"uniform mat4 u_projectionMatrix;\r\n"
	"\r\n"
	"// Varyings\r\n"
	"varying vec4 v_color;\r\n"
	"\r\n"
	"\r\n"
	"void main()\r\n"
	"{\r\n"
	"    gl_Position = u_projectionMatrix * vec4(a_position, 1);\r\n"
	"    v_color = a_color;\r\n"
	"}\r\n"
};

const char *intshd_spritecol_frag = {
	"#if defined(OPENGL_ES) || defined(GL_ES)\r\n"
	"#extension GL_OES_standard_derivatives : enable\r\n"
	"#ifdef GL_FRAGMENT_PRECISION_HIGH\r\n"
	"precision highp float;\r\n"
	"#else\r\n"
	"precision mediump float;\r\n"
	"#endif\r\n"
	"#endif\r\n"
	"\r\n"
	"// Varyings\r\n"
	"varying vec4 v_color;\r\n"
	"\r\n"
	"\r\n"
	"void main()\r\n"
	"{\r\n"
	"    gl_FragColor = v_color;\r\n"
	"}\r\n"
};


const char* intshd_dummy = {
	""
};

#endif

