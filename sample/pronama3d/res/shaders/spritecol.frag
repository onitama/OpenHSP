#if defined(OPENGL_ES) || defined(GL_ES)
#extension GL_OES_standard_derivatives : enable
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#else
precision mediump float;
#endif

// Varyings
varying vec4 v_color;


void main()
{
    gl_FragColor = v_color;
}