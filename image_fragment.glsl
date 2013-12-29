
uniform sampler2D imageSampler;

uniform mediump mat4 matrix;
uniform highp float exposure;
uniform highp float img_gamma;
uniform highp float gamma;


varying highp vec4 f_pos;
varying highp vec4 f_uv;

void main(void)
{
    vec4 color = texture2D(imageSampler, f_uv.xy).rgba;
    vec3 linear_color = vec3(pow(color.r, img_gamma), pow(color.g, img_gamma), pow(color.b, img_gamma));
    color = vec4(exposure * linear_color, color.a);
    gl_FragColor = vec4(pow(color.r, 1.0/gamma), pow(color.g, 1.0/gamma), pow(color.b, 1.0/gamma), color.a);
}
