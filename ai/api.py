from fastapi import FastAPI
from fastapi.responses import JSONResponse
import uvicorn

from DataToImages import DataToImages

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

    python will isolate the characters and export them as PNG

    have seperate file, jupyter or py, run a model based on these images

    in the future,
        capture the model response 
        return it as jsonresponse
    '''
    # print("Received data:", data)

    filepath = DataToImages(data, filename="test_draw", path="PreTrainedModels")
    return JSONResponse(content={"status": "success", "message": f"Data received successfully: {data}"})



if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)