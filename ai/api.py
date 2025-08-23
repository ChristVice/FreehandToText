from fastapi import FastAPI
from fastapi.responses import JSONResponse
import uvicorn

import os
import time
from PIL import Image, ImageDraw

app = FastAPI(title="Handwriting to Text API")

stroke_data = {}

@app.get("/")
async def root():
    return {"message": "Handwriting to Text API is running"}

@app.get("/prediction")
async def predict_text():
    # This will call your AI processing function
    result = process_handwriting()
    return {"message": result}

@app.get("/data")
async def get_data():
    # Return the stroke data received
    return JSONResponse(content={"status": "success", "data": stroke_data})

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
    stroke_data_to_png(data, "test_draw")


    print("Received data:", data)
    return JSONResponse(content={"status": "success", "message": f"Data received successfully: {data}"})

def process_handwriting():
    # Placeholder for your AI model processing
    # This is where you'll add your CNN-LSTM model logic
    return "this is a return statement from the ai model"




# FUNCTIONS FOR TURNING POSTED DATA INTO AN IMAGE THAT SAVES TO DIRECTORY

def draw_image(canvas: Image.Image, 
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

def process_strokes_to_image(data: dict) -> Image.Image:
    """
    Main function to convert stroke data to PIL Image.
    
    Args:
        data: Raw stroke data from API
        
    Returns:
        PIL Image with drawn strokes
    """
    # Parse data
    canvas_width = data.get("canvas_width", 800)
    canvas_height = data.get("canvas_height", 600)
    strokes = data.get("strokes", [])
    
    # Create canvas
    canvas = Image.new(mode="RGB", size=(canvas_width, canvas_height), color="white")
    
    # Draw strokes
    image_with_strokes = draw_image(canvas, strokes)
    
    return image_with_strokes

def save_image_as_png(image: Image.Image, 
                      filename: str = "handwriting_temp.png",
                      output_dir: str = None) -> str:
    """
    Save PIL Image as PNG file.

    Args:
        image: PIL Image to save
        filename: Name of the output file
        output_dir: Directory to save the file (defaults to current project folder)

    Returns:
        Full path to the saved file
    """
    # Use current working directory if output_dir is None
    if output_dir is None:
        output_dir = os.getcwd()

    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)

    # Full path
    filepath = os.path.join(output_dir, filename)

    # Save image
    image.save(filepath, "PNG")

    return filepath

def stroke_data_to_png(data: dict, 
                      output_filename: str = None,
                      output_dir: str = None) -> str:
    """
    Convert stroke data to PNG file.
    
    Args:
        data: Stroke data from API
        output_filename: Custom filename, (if None then name is handwriting.png)
        output_dir: Directory to save the PNG
        
    Returns:
        Path to the saved PNG file
    """
    # Generate filename if not provided
    if output_filename is None:
        output_filename = "handwriting.png"
    
    # Process strokes to image
    image = process_strokes_to_image(data)
    
    # Use current working directory if output_dir is None
    if output_dir is None:
        output_dir = os.path.join(os.getcwd(), "PreTrainedModels")

    # Save as PNG
    png_path = save_image_as_png(image, output_filename+".png", output_dir)
    
    return png_path






if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)