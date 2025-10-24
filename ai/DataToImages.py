import os
import cv2
import numpy as np
from PIL import Image

# CLASS FOR TURNING POSTED DATA INTO AN IMAGE THAT SAVES TO DIRECTORY
class DataToImages:

    def __new__(cls, size: list[int], data: dict, filename: str = "", path: str = "", resize: list[int] = [256, 64], save:bool = False) -> Image:
        
        # Step 1. Create blank canvas (white background)
        canvas_h = int(data.get("canvas_height", (size[1] if size else 28)))
        canvas_w = int(data.get("canvas_width",  (size[0] if size else 28)))
        canvas = np.full((canvas_h, canvas_w), fill_value=255, dtype=np.uint8)

        # Step 2. Draw strokes on canvas
        for stroke in data.get("strokes", []):
            for i in range(1, len(stroke)):
                x1, y1 = stroke[i-1]
                x2, y2 = stroke[i]
                # Ensure coordinates are integers
                x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
                cv2.line(canvas, (x1, y1), (x2, y2), color=0, thickness=4)
        
        # Create to PIL
        pil_img = Image.fromarray(canvas).convert("RGB")

        # Choose resampling filter compatibly across Pillow versions
        if hasattr(Image, "Resampling"):
            resample_filter = Image.Resampling.LANCZOS
        else:
            # fallback for older Pillow versions
            resample_filter = getattr(Image, "LANCZOS", Image.ANTIALIAS)

        pil_img = pil_img.resize(resize, resample_filter).convert('L')

        # Optionally save to disk
        if save and len(filename) > 0:
            output_dir = os.path.join(os.getcwd(), path)
            os.makedirs(output_dir, exist_ok=True)
            out_path = os.path.join(output_dir, f"{filename}.png")
            pil_img.save(out_path)

        # Return PIL image instead of an instance
        print(f"Finished making PNG")
        return pil_img
