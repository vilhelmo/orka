
uniform highp vec4 color;
uniform highp float exposure;
uniform highp float img_gamma;
uniform highp float gamma;

void main(void)
{
    vec3 linear_color = vec3(pow(color.r, img_gamma), pow(color.g, img_gamma), pow(color.b, img_gamma));
    vec3 thecolor = exposure * linear_color;
    gl_FragColor = vec4(pow(thecolor.r, 1.0/gamma), pow(thecolor.g, 1.0/gamma), pow(thecolor.b, 1.0/gamma), color.a);
}
