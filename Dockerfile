# Use Ubuntu as base
FROM ubuntu:22.04

# Install system dependencies
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-venv \
    curl

# Set working directory
WORKDIR /app

# Copy entire project
COPY . .

# Build the C++ SFML application
# WORKDIR /app/ui
RUN g++ ui/main.cpp -o ui/app -lsfml-graphics -lsfml-window -lsfml-system

# Install Python dependencies
# WORKDIR /app/ai
RUN pip3 install --no-cache-dir -r requirements.txt

# Set default command (can be adjusted as needed)
# This will run both Python backend and C++ UI in parallel

CMD ["./ui/app"]
