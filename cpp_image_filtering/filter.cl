__kernel void low_pass_filter(__global const unsigned char* input, __global unsigned char* output, int width, int height) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    if (x >= width || y >= height) return;

    // Simple averaging filter (3x3 kernel)
    int sum = 0;
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                sum += input[ny * width + nx];
                count++;
            }
        }
    }
    output[y * width + x] = sum / count;
}

__kernel void high_pass_filter(__global const unsigned char* input, __global unsigned char* output, int width, int height) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    if (x >= width || y >= height) return;

    // Simple high-pass filter (using the low-pass result)
    int low_pass_value = 0;
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                low_pass_value += input[ny * width + nx];
                count++;
            }
        }
    }
    low_pass_value /= count;
    output[y * width + x] = input[y * width + x] - low_pass_value;
}
