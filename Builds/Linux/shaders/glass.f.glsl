varying vec4 graph_coord;

void main(void) {
	gl_FragColor = vec4(0.5, 0.0, sqrt(graph_coord[2]), 0.5);
}
