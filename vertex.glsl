attribute highp vec4 vertex;
attribute highp vec4 uv;

uniform mediump mat4 matrix;
uniform mediump mat4 inv_matrix;

varying highp vec4 f_pos;
varying highp vec4 f_uv;

void main(void) {
    f_pos = matrix * vertex;
    f_uv = vec4(uv.x, 1.0 - uv.y, 0.0, 0.0);
    gl_Position = f_pos;
}