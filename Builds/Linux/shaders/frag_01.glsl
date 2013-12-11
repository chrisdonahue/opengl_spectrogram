varying vec4 graph_coord;

void main(void) {
	gl_FragColor = graph_coord / 2.0 + 0.5;
}
