import uvicorn
import tensorflow as tf
import numpy as np
from PIL import Image
from fastapi import FastAPI
from fastapi.responses import JSONResponse
from DataToImages import DataToImages
from utils import get_decoded_texts



app = FastAPI(title="Handwriting to Text API")
model = tf.keras.models.load_model("./PreTrainedModels/TrainedModels/new_full_model_27val_12tra.keras", compile=False)

@app.get("/")
async def root():
    return {"message": "Handwriting to Text API is running"}

@app.post("/data")
async def post_data(data: dict):
    # Here you can process the incoming data
    # For example, you might save it or pass it to your AI model

    '''
    data
    {'canvas_height': 150, 
     'canvas_width': 800, 
     'strokes': [[[262.0, 78.0], [262.0, 78.0], [263.0, 78.0], [263.0, 78.0], [263.0, 78.0], [263.0, 78.0], [264.0, 78.0], [264.0, 78.0], [264.0, 78.0]]]
    }
    '''
    model_input_parameters = {
        "height" : 64,
        "width" : 256
    }

    try:
        user_image = DataToImages(size=[data.get("canvas_width", 256), data.get("canvas_height", 64)], 
                            data=data, 
                            filename="handwriting", 
                            path="PreTrainedModels", 
                            resize=[model_input_parameters["width"], model_input_parameters["height"]], 
                            save=True)

        print("Received data:", data)

        img_arr = np.array(user_image)
        img_arr = img_arr.reshape(1, model_input_parameters["height"], model_input_parameters["width"], 1)  # Add batch and channel dimensions
        img_arr = img_arr.astype("float32") / 255.0

        # Predict
        raw_prediction = model.predict(img_arr)
        prediction = get_decoded_texts(raw_predictions=raw_prediction)

        return JSONResponse(content={"status": "success", "prediction": f"{prediction[0]}"})
    
    except Exception as e:
        return JSONResponse(content={"status": "error", "prediction": f" "})


if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)