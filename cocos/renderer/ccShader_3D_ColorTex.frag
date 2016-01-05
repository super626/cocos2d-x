
const char* cc3D_ColorTex_frag = STRINGIFY(

\n#ifdef GL_ES\n
varying mediump vec2 TextureCoordOut;
\n#else\n
varying vec2 TextureCoordOut;
\n#endif\n
uniform vec4 u_color;
                                           
\n#ifdef RECEIVE_SHADOW\n
uniform highp sampler2D u_shadow;
varying highp vec4      vProjCoord;
\n#endif\n

void main(void)
{
\n#ifdef RECEIVE_SHADOW\n
    highp float comp = (vProjCoord.z) - 0.03;
    highp float depth = texture2DProj(u_shadow, vProjCoord).r;
    lowp float shadowVal = comp <= depth ? 1.0 : 0.4;
    gl_FragColor = texture2D(CC_Texture0, TextureCoordOut) * u_color * shadowVal;
\n#else\n
    gl_FragColor = texture2D(CC_Texture0, TextureCoordOut) * u_color;
\n#endif\n
    
    
}
);
