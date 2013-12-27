
//varying highp vec4 f_color;
uniform highp vec4 color;

void main(void)
{
    //gl_FragColor = f_color;
    gl_FragColor = color;
}
