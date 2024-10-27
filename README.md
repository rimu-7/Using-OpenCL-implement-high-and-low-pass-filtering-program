# Image Processing with OpenCL and OpenCV

This project implements low-pass and high-pass filtering on images using OpenCL for parallel processing and OpenCV for image manipulation. The program reads an input image, applies both filters, and saves the results as output images.

## Requirements
- C++ Compiler (g++ or similar)
- OpenCL Development Libraries
- OpenCV Development Libraries

## Installation

1. **Install OpenCV**: On Arch Linux, you can install OpenCV using:
   `bash`
    sudo pacman -S opencv

2. **Install OpenCL**: Ensure you have the OpenCL development libraries installed. You can install them using:
`bash`
    sudo pacman -S opencl-headers

3. **Compile the Program:**
Use the following command to compile the C++ program:
`bash`
    g++ -o image_processing main.cpp -lOpenCL $(pkg-config --cflags --libs opencv4)

    **For python**
    - pip install opencv
    - pip install numpy    

## Usage
To run the program, use the following command format:
`bash`
    ./image_processing <input_image> <output_low_pass> <output_high_pass>

## Example
Here is an example command to run the program:
`bash`
    ./image_processing input.jpg output_low_pass.jpg output_high_pass.jpg

