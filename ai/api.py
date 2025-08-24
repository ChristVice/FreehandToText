from fastapi import FastAPI
from fastapi.responses import JSONResponse
import uvicorn

import os
from PIL import Image, ImageDraw

app = FastAPI(title="Handwriting to Text API")

@app.get("/")
async def root():
    return {"message": "Handwriting to Text API is running"}

@app.post("/data")
async def post_data(data: dict):
    # Here you can process the incoming data
    # For example, you might save it or pass it to your AI model

    '''
    get canvas details, including strokes

    python makes an png based on this information

    have seperate file, jupyter or py, run a model based on this image

    in the future,
    capture the response here and then return it as jsonresponse
    '''
    filepath = DataToImage(data, filename="test_draw", path="PreTrainedModels")

    print("Received data:", data)
    return JSONResponse(content={"status": "success", "message": f"Data received successfully: {data}"})


# CLASS FOR TURNING POSTED DATA INTO AN IMAGE THAT SAVES TO DIRECTORY
class DataToImage:

    def __init__(self, data: dict, filename: str = "output_img", path: str = ""):
        self.data = data
        self.filename = filename
        self.path = path

        self.stroke_data_to_png()
        

    def stroke_data_to_png(self) -> str:
        """
        Convert stroke data to PNG file.
            
        Returns:
            Path to the saved PNG file
        """
        
        # Process strokes to image
        image = self.process_strokes_to_image()
        
        # Save as PNG
        png_path = self.save_image_as_png(image)
        
        return png_path

    def process_strokes_to_image(self) -> Image.Image:
        """
        Main function to convert stroke data to PIL Image.

        Returns:
            PIL Image with drawn strokes
        """
        # Parse data
        canvas_width = self.data.get("canvas_width", 800)
        canvas_height = self.data.get("canvas_height", 600)
        strokes = self.data.get("strokes", [])
        
        # Create canvas
        canvas = Image.new(mode="RGB", size=(canvas_width, canvas_height), color="white")
        
        # Draw strokes
        image_with_strokes = self.draw_image(canvas, strokes)
        
        return image_with_strokes

    def draw_image(self, canvas: Image.Image, 
                            strokes: list[list[tuple[int, int]]],
                            stroke_color: str = "black",
                            stroke_width: int = 3) -> Image.Image:
        """
        Draw strokes on the canvas.
        
        Args:
            canvas: PIL Image to draw on
            strokes: List of stroke paths (each stroke is list of (x,y) points)
            stroke_color: Color of the strokes
            stroke_width: Width of the stroke lines
            
        Returns:
            PIL Image with strokes drawn
        """
        draw = ImageDraw.Draw(canvas)

        for stroke in strokes:
            if len(stroke) < 2:
                continue  # Skip single points
            
            # Draw lines between consecutive points
            for i in range(len(stroke) - 1):
                start_point = stroke[i]
                end_point = stroke[i + 1]
                draw.line([start_point, end_point], fill=stroke_color, width=stroke_width)
        
        return canvas

    def save_image_as_png(self, 
                        image: Image.Image) -> str:
        """
        Save PIL Image as PNG file.

        Args:
            image: PIL Image to save

        Returns:
            Full path to the saved file
        """
        # Generate filename if not provided
        if self.filename is None:
            self.filename = "output_img.png"

        # Use current working directory
        output_dir = os.path.join(os.getcwd(), self.path)

        # Ensure output directory exists
        os.makedirs(output_dir, exist_ok=True)

        # Full path
        filepath = os.path.join(output_dir, self.filename+".png")

        # Save image
        image.save(filepath, "PNG")

        return filepath







if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)