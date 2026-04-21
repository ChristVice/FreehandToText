# FreehandToText

`FreehandToText` is a desktop handwriting-recognition prototype built for a senior project. It combines a native **C++/SFML** drawing interface with a **Python/FastAPI** inference service. The UI captures freehand pen strokes, sends them to the backend as stroke coordinates, and displays the text predicted by a trained TensorFlow model.

This repository is best described as a **handwriting-to-text application**, not a general AI drawing assistant.

## What The Project Does

- Lets a user draw handwritten text in a desktop canvas
- Groups mouse input into strokes
- Sends stroke data to a local API as JSON
- Converts stroke paths into a raster image on the backend
- Resizes and normalizes the image for model inference
- Runs a pretrained TensorFlow model to predict text
- Displays the decoded prediction in the lower half of the desktop app

## Architecture

### UI (`ui/`)

The frontend is a native SFML application written in C++.

- Top half of the window is the drawing area
- Bottom half shows the predicted text and control buttons
- Supports:
  - drawing with the mouse
  - `Undo [Z]`
  - `Clear [C]`
  - `Close [Esc]`
- Uses a background worker thread to POST stroke data to the backend
- Uses `libcurl` for HTTP requests and `nlohmann/json` for JSON handling

### AI Service (`ai/`)

The backend is a FastAPI service that loads a pretrained Keras model at startup.

Main flow in [`ai/api.py`](/Users/chvicencio/Desktop/Projects/FreehandToText/ai/api.py):

1. Receive JSON containing `canvas_width`, `canvas_height`, and `strokes`
2. Convert stroke coordinates into an image via [`ai/DataToImages.py`](/Users/chvicencio/Desktop/Projects/FreehandToText/ai/DataToImages.py)
3. Resize the image to `256x64` grayscale
4. Normalize and reshape the image for inference
5. Run the TensorFlow model
6. Decode model output into text with CTC beam search in [`ai/utils.py`](/Users/chvicencio/Desktop/Projects/FreehandToText/ai/utils.py)

## Repository Layout

```text
.
├── README.md
├── TODO.txt
├── ai
│   ├── api.py
│   ├── DataToImages.py
│   ├── utils.py
│   ├── requirements.txt
│   ├── Dockerfile
│   └── PreTrainedModels/
└── ui
    ├── main.cpp
    └── assets/
```

## Data Flow

The current implementation sends data in this shape:

```json
{
  "canvas_width": 800,
  "canvas_height": 150,
  "strokes": [
    [[262.0, 78.0], [263.0, 78.0], [264.0, 78.0]]
  ]
}
```

Each stroke is a list of 2D points captured from the user’s drawing motion.

## Current Status

What is implemented now:

- Desktop drawing UI
- Undo, clear, and close controls
- API integration between UI and backend
- Stroke-to-image conversion
- Pretrained TensorFlow model loading and inference
- Prediction rendering in the UI

What is not clearly implemented in this repo yet:

- Full build automation for the C++ app
- A documented training pipeline for the model used in production
- Save/export workflows for user drawings
- Copy/paste or text editing in the result area
- Request throttling/debouncing in the UI worker
- Separate documented support for cursive vs. print models

## Running The Backend

From the `ai/` directory:

```bash
pip install -r requirements.txt
python api.py
```

The API starts on `http://localhost:8000` by default.

There is also a Dockerfile in [`ai/Dockerfile`](/Users/chvicencio/Desktop/Projects/FreehandToText/ai/Dockerfile) for containerizing the Python service.

## Running The UI

The UI is implemented in [`ui/main.cpp`](/Users/chvicencio/Desktop/Projects/FreehandToText/ui/main.cpp), but this repository does not currently include a build script or `CMakeLists.txt`. To run it, you will need a local C++ toolchain with:

- SFML
- libcurl
- `nlohmann/json`

The UI reads the backend URL from `API_URL`. If `API_URL` is not set, it defaults to:

```text
http://localhost:8000
```

## Notes On Accuracy And Scope

The repo suggests a broader long-term goal around handwriting and cursive recognition, but the checked-in implementation is currently focused on:

- collecting freehand stroke input
- converting it into an image
- sending it to a local inference service
- decoding a single model prediction back into text

That narrower scope is what this README documents.

## Future Improvements

Based on the current code and TODO list, the next practical improvements are:

- reduce redundant API calls while drawing changes are frequent
- document or automate model management
- add reproducible build instructions for the UI
- improve result interaction, such as copy/edit support
- define evaluation metrics for handwriting and cursive recognition separately
