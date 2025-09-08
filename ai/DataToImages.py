import os
from PIL import Image, ImageDraw

import cv2
import numpy as np

# CLASS FOR TURNING POSTED DATA INTO AN IMAGE THAT SAVES TO DIRECTORY
class DataToImages:

    def __init__(self, data: dict, filename: str = "output_img", path: str = ""):
        self.data = data
        self.filename = filename
        self.path = path

        self.canvas_size = 28

        self.export_characters_img(x_gap_threshold=3)
        
    def export_characters_img(self, folder: str = "", x_gap_threshold: int = 5):

        # Step 1. Create blank canvas (white background)
        canvas = np.full((self.data["canvas_height"], self.data["canvas_width"]), fill_value=255, dtype=np.uint8)

        # Step 2. Draw strokes on canvas
        for stroke in self.data.get("strokes", []):
            for i in range(1, len(stroke)):
                x1, y1 = stroke[i-1]
                x2, y2 = stroke[i]
                # Ensure coordinates are integers
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
                cv2.line(canvas, (x1, y1), (x2, y2), color=0, thickness=5)
        
        # Export full canvas
        output_dir = os.path.join(os.getcwd(), self.path)
        os.makedirs(output_dir, exist_ok=True)
        cv2.imwrite(os.path.join(output_dir, "fullhandwriting.png"), canvas)

        # Step 3. Compute bounding boxes of each stroke
        stroke_boxes = []
        for stroke in self.data.get("strokes", []):
            xs = [p[0] for p in stroke]
            ys = [p[1] for p in stroke]
            bbox = (min(xs), max(xs), min(ys), max(ys))  # (min_x, max_x, min_y, max_y)
            stroke_boxes.append(bbox)

        # Step 4. Sort strokes left-to-right
        stroke_boxes = sorted(stroke_boxes, key=lambda b: b[0])

        # Step 5. Group strokes into letters
        letters = []
        current_group = [stroke_boxes[0]]

        for i in range(1, len(stroke_boxes)):
            prev = stroke_boxes[i-1]
            curr = stroke_boxes[i]

            gap = curr[0] - prev[1]  # distance between prev.max_x and curr.min_x

            if gap < x_gap_threshold:
                current_group.append(curr)
            else:
                letters.append(current_group)
                current_group = [curr]

        letters.append(current_group)  # add last group

        # Step 6. Merge each group into one bounding box
        merged_boxes = []
        for group in letters:
            min_x = min(b[0] for b in group)
            max_x = max(b[1] for b in group)
            min_y = min(b[2] for b in group)
            max_y = max(b[3] for b in group)
            merged_boxes.append((min_x, max_x, min_y, max_y))


        # Find or create folder for characters drawings
        chars_output_dir = os.path.join(os.getcwd(), f"{self.path}/Drawings")
        os.makedirs(chars_output_dir, exist_ok=True)

        # Step 7. Crop and export
        for i, (min_x, max_x, min_y, max_y) in enumerate(merged_boxes):
            min_x, max_x, min_y, max_y = int(round(min_x)), int(round(max_x)), int(round(min_y)), int(round(max_y))

            letter_img = canvas[min_y:max_y+1, min_x:max_x+1]
            letter_img = self.resize_to_canvas(img=letter_img)
            # cv2.imwrite(f"letter_{i}.png", letter_img)
            cv2.imwrite(os.path.join(chars_output_dir, f"letter_{i}.png"), letter_img)

        print(f"Exported {len(merged_boxes)} letters as PNGs")

    
    def resize_to_canvas(self, img):
        """Resize a segmented letter to canvas_size x canvas_size with padding, preserving aspect ratio."""

        canvas_size = self.canvas_size

        h, w = img.shape

        # Scale the longer side to (canvas_size - 8) pixels (leaving padding)
        target = canvas_size - 8
        if h > w:
            new_h = target
            new_w = int(w * (target / h))
        else:
            new_h = int(h * (target / w))
            new_w = target

        # Prevent zero or negative dimensions
        if new_w <= 0 or new_h <= 0:
            print(f"Warning: Skipping resize due to non-positive dimensions: new_w={new_w}, new_h={new_h}, original shape={img.shape}")
            return np.full((canvas_size, canvas_size), dtype=np.uint8, fill_value=255)  # Return blank canvas

        resized = cv2.resize(img, (new_w, new_h), interpolation=cv2.INTER_AREA)

        # Create blank canvas and paste resized letter in center
        canvas = np.full((canvas_size, canvas_size), dtype=np.uint8, fill_value=255)
        y_offset = (canvas_size - new_h) // 2
        x_offset = (canvas_size - new_w) // 2
        canvas[y_offset:y_offset+new_h, x_offset:x_offset+new_w] = resized

        return canvas