
const char* cc3D_ColorTex_frag = STRINGIFY(

\n#ifdef GL_ES\n
varying mediump vec2 TextureCoordOut;
\n#else\n
varying vec2 TextureCoordOut;
\n#endif\n
uniform vec4 u_color;
                                           
#ifdef RECEIVE_SHADOW
uniform highp sampler2D u_Shadow;
varying highp vec4      vProjCoord;
#endif

void main(void)
{
#ifdef RECEIVE_SHADOW
    highp float comp = (vProjCoord.z / vProjCoord.w) - 0.03;
    highp float depth = texture2DProj(u_Shadow, vProjCoord).r;
    lowp float shadowVal = comp <= depth ? 1.0 : fAmbient;
    gl_FragColor = texture2D(CC_Texture0, TextureCoordOut) * u_color * shadowVal;
#else
    gl_FragColor = texture2D(CC_Texture0, TextureCoordOut) * u_color;
#endif
    
    
}
);
