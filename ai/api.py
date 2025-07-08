from fastapi import FastAPI
from fastapi.responses import JSONResponse
import uvicorn

app = FastAPI(title="Handwriting to Text API")

stroke_data = {}

@app.get("/")
async def root():
    return {"message": "Handwriting to Text API is running"}

@app.get("/prediction")
async def predict_text():
    # This will call your AI processing function
    result = process_handwriting()
    return {"prediction": result}

@app.get("/data")
async def get_data():
    # Return the stroke data received
    return JSONResponse(content={"status": "success", "data": stroke_data})

@app.post("/data")
async def post_data(data: dict):
    # Here you can process the incoming data
    # For example, you might save it or pass it to your AI model
    stroke_data = data
    print("Received data:", data)
    return JSONResponse(content={"status": "success", "message": f"Data received successfully: {stroke_data}"})

def process_handwriting():
    # Placeholder for your AI model processing
    # This is where you'll add your CNN-LSTM model logic
    return "this is a return statement from the ai model"

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)