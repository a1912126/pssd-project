from PIL import Image
import numpy as np

# Load the image (replace 'city_map.png' with your file name)
image = Image.open("city_map.png").convert("RGB")

# Resize the image to match the grid (example: resize to 900x600)
image = image.resize((900, 600))  # Resize to fit your grid

# Convert the image to a numpy array
pixels = np.array(image)

# Create a binary map (1 for white, 0 for black)
binary_map = np.zeros((600, 900), dtype=int)

for y in range(600):
    for x in range(900):
        # Check if the pixel is white (255, 255, 255)
        if np.array_equal(pixels[y, x], [255, 255, 255]):  # White pixel
            binary_map[y, x] = 0  # Mark as road
        else:
            binary_map[y, x] = 1  # Mark as building

# Save the binary map to a file (map.txt)
np.savetxt("map.txt", binary_map, fmt="%d")
