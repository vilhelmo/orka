
uniform sampler2D imageSampler;

uniform mediump mat4 matrix;
uniform mediump mat4 inv_matrix;

varying highp vec4 f_pos;
varying highp vec4 f_uv;

void main(void)
{
    gl_FragColor = texture2D(imageSampler, f_uv.xy).rgba;
    //gl_FragColor = vec4(texture2D(imageSampler, f_uv.xy).rgb, 1.0);
    //gl_FragColor = vec4(f_uv.xy, 0.0, 1.0);
}
