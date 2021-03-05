#if defined(OPENGL_ES) || defined(GL_ES)
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#endif

///////////////////////////////////////////////////////////
// Uniforms
uniform sampler2D u_texture;

///////////////////////////////////////////////////////////
// Varyings
varying vec2 v_texCoord;
varying vec4 v_color;
uniform float u_ratio;

void main()
{
	vec4 color = texture2D(u_texture, v_texCoord);

	// check whether fragment output is higher than threshold, if so output as brightness color
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness < u_ratio) {
		color = vec4(0.0, 0.0, 0.0, 1.0);
	}
	//color.r = max( color.r - u_ratio, 0.0 ) + u_ratio;
	//color.g = max( color.g - u_ratio, 0.0 ) + u_ratio;
	//color.b = max( color.b - u_ratio, 0.0 ) + u_ratio;
	//color.a = 1.0;
	gl_FragColor = v_color * color;

}

