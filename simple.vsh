attribute vec2 p;
attribute vec4 color;

varying vec4 v_color;
uniform mat3 transform;

void main() {

	gl_Position.xy = (transform * vec3(p, 1.0)).xy;
	gl_Position.zw = vec2(0.0, 1.0);
	v_color = color;
}