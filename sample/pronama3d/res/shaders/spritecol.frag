#if defined(OPENGL_ES) || defined(GL_ES)
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#endif

// Varyings
varying vec4 v_color;


void main()
{
    gl_FragColor = v_color;
}