git clone https://github.com/jploski/ggml falcon-ggml
cd falcon-ggml
git checkout falcon40b
mkdir build && cd build && cmake -DGGML_CUBLAS=ON -DCMAKE_CUDA_COMPILER=/usr/local/cuda-12.1/bin/nvcc .. && cmake -DGGML_CUBLAS=ON -DCMAKE_CUDA_COMPILER=/usr/local/cuda-12.1/bin/nvcc --build . --config Release
