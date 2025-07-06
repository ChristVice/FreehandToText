# Use Ubuntu as base
FROM python:3.12-slim

# Set working directory
WORKDIR /app

COPY requirements.txt .

RUN pip3 install --no-cache-dir -r requirements.txt

COPY . .

CMD ["python3", "main.py"]
