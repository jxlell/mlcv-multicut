import numpy as np
from PIL import Image

def images_are_exactly_equal(img_path1, img_path2):
    """
    Compare two images for exact pixel equality.
    Returns True only if all pixels match exactly.
    """
    img1 = Image.open(img_path1).convert('RGB')
    img2 = Image.open(img_path2).convert('RGB')

    if img1.size != img2.size:
        return False

    arr1 = np.array(img1)
    arr2 = np.array(img2)

    return np.array_equal(arr1, arr2)

# Example usage:
print(images_are_exactly_equal(
    '/Users/jalell/Documents/GitHub/lossless-benchmark/amazon_jxl.png',
    '/Users/jalell/Documents/GitHub/lossless-benchmark/amazon.com.png'
))
