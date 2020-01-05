
///////////////////////////////////////////////////////////
// Atributes
attribute vec4 a_position;
attribute vec2 a_texCoord;

///////////////////////////////////////////////////////////
// Uniforms
uniform mat4 u_worldViewProjectionMatrix;
uniform vec4 u_voffset;

#if defined(TEXTURE_REPEAT)
uniform vec2 u_textureRepeat;
#endif

#if defined(TEXTURE_OFFSET)
uniform vec2 u_textureOffset;
#endif

#if defined(CLIP_PLANE)
uniform mat4 u_worldMatrix;
uniform vec4 u_clipPlane;
#endif

///////////////////////////////////////////////////////////
// Varyings
varying vec3 v_texCoord;

#if defined(CLIP_PLANE)
varying float v_clipDistance;
#endif

void main()
{
    vec4 position = a_position;
    gl_Position = u_worldViewProjectionMatrix * position;
//    gl_Position = gl_Position.xyww;

//    gl_TexCoord[0] = gl_Vertex;
//    v_texCoord = a_texCoord;
//    v_texCoord = normalize(position.xyz);
    v_texCoord.xyz = position.xyz + u_voffset.xyz;
    
    #if defined(TEXTURE_REPEAT)
    v_texCoord *= u_textureRepeat;
    #endif
    
    #if defined(TEXTURE_OFFSET)
    v_texCoord += u_textureOffset;
    #endif
    
    #if defined(CLIP_PLANE)
    v_clipDistance = dot(u_worldMatrix * position, u_clipPlane);
    #endif
}
