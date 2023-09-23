#! /usr/bin/env sh
# skipped_samples 
cmake -S . -B out_linux \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=True \
    -DCMAKE_BUILD_TYPE=Debug \
    -DOpenGL_GL_PREFERENCE=LEGACY 
cmake --build out_linux --config Debug -j 7