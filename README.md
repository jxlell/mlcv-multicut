# Multicut Image Compression Implementation

This project implements an image compression method based on multicut encodings. 
This code was written in the course of the Master's thesis 
***Encodings of Graph Multicuts and Their Application to Image Compression***, submitted to the examination office on June 16, 2025. 

## 👤 Author
**Jalell Fobugwe** 
> - Uni Mail: [jalell_che.fobugwe@mailbox.tu-dresden.de](mailto:jalell_che.fobugwe@mailbox.tu-dresden.de)
> - Private Mail: [jalell.fobugwe@gmail.com](mailto:jalell.fobugwe@gmail.com)

## 📦 Requirements

- C++17 or newer
- [CMake](https://cmake.org/) ≥ 3.10
- [OpenCV](https://opencv.org/)
- [zlib 1.3.1](https://zlib.net/manual.html)

## 🔨 Build Instructions 
- a fresh build is required before running for the first time
### CMake
<pre><code>
mkdir -p cmake-build
cd cmake-build
cmake ..
cmake --build .
</code></pre>

### Running the Executable
- move an image file to <code>/cmake-build</code>
<pre><code>
./multicut_compression [img].png
</code></pre>
- executable must be run from the <code>cmake-build</code> folder
- a <code>[compressed_img].bin</code> is going to be created

## 📁 Code Structure 
- <code>main.cpp</code>: invokes compression and decompression process, file handling, constructs .bin file
- <code>compress.cpp</code>: constructs the compressed bitsring
- <code>decompress.cpp</code>: reconstructs the image from the .bin file
- <code>mc.cpp</code>: handles edge traversal logic
- <code>Util.cpp</code>: provides helper functionalities relating to image pixel operations, graph traversal
color management and bitstring serialization/parsing.
- <code>main_png_jxl.cpp</code>: evaluation logic for obtaining the PNG and JPEG XL results 

## ℹ️ Notice 
Parts of the code were written with the help of Large Language Models such as ChatGPT and Github Copilot. The Huffman functionality was adapted from [a Github repo](https://github.com/mr-ayush-agrawal/Hacktoberfest/blob/9e5d9c36121664d988d2072cd8eafc476c008c9e/Most_Useful_Algorithms/HuffmanCoding.cpp).

