import cv2 # type: ignore
import numpy as np

def low_pass_filter(image):
    # Apply a Gaussian blur for low-pass filtering
    return cv2.GaussianBlur(image, (5, 5), 0)

def high_pass_filter(image, low_passed):
    # Subtract the low-pass filtered image from the original image
    return cv2.subtract(image, low_passed)

def process_image(input_file_path, output_low_pass, output_high_pass):
    # Read the input image
    img = cv2.imread(input_file_path, cv2.IMREAD_GRAYSCALE)
    
    if img is None:
        print("Error: Could not open or find the image.")
        return

    # Apply low-pass filter
    low_passed = low_pass_filter(img)
    cv2.imwrite(output_low_pass, low_passed)

    # Apply high-pass filter
    high_passed = high_pass_filter(img, low_passed)
    cv2.imwrite(output_high_pass, high_passed)

    print(f"Low-pass image saved as: {output_low_pass}")
    print(f"High-pass image saved as: {output_high_pass}")

if __name__ == "__main__":
    input_image = "input.jpg"
    output_low_pass = "py_low_pass.jpg"
    output_high_pass = "py_high_pass.jpg"

    process_image(input_image, output_low_pass, output_high_pass)
