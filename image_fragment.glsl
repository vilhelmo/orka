
uniform sampler3D lutSampler;
uniform sampler2D imageSampler;

uniform mediump mat4 matrix;
uniform highp float exposure;

varying highp vec4 f_pos;
varying highp vec4 f_uv;

void main(void)
{
    vec4 color = texture2D(imageSampler, f_uv.xy).rgba;
    color = vec4(exposure * color.rgb, color.a);
    gl_FragColor = OCIODisplay(color, lutSampler);
}
