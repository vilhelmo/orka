
attribute highp vec4 vertex;
//attribute highp vec4 color;

uniform mediump mat4 matrix;

//varying highp vec4 f_color;

void main(void) {
    gl_Position = matrix * vertex;
    //f_color = color;
}
