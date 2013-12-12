#!/bin/bash

set -e

#<<COMMENT
#valgrind --tool=callgrind \
#gdb --args \
./build/open_gl_spectrogram \
    --fft_size 1024 \
    --fft_overlap 0 \
    --fft_window_type rectangle \
    --vertex_shader_file_path ./shaders/graph.v.glsl \
    --fragment_shader_file_path ./shaders/graph.f.glsl \
    --audio_file_path ./audio/valve_intro.wav 
#COMMENT

# DEBUG MEMORY
<<COMMENT
valgrind -v --leak-check=full --show-reachable=yes --log-file="2GenValgrind.txt"\
  > 2GenValgrind.txt
COMMENT

# DEBUG HEAP
<<COMMENT
valgrind -v --tool=massif --log-file="3GenValgrindMassif.txt"\
  > 3GenValgrindMassif.txt
COMMENT

# DEBUG PERF
<<COMMENT
valgrind --tool=callgrind \
    > 2GenCallgrind.txt
COMMENT

set +e
