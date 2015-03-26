#ifdef GL_ES
precision mediump float;
#endif

varying vec3 v_reflect;
uniform samplerCube u_Env;

void main(void)
{
    gl_FragColor = textureCube(u_Env, v_reflect);
}