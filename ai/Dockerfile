# Use Ubuntu as base
FROM python:3.10-slim

COPY requirements.txt .

# Set working directory
WORKDIR /app

RUN pip3 install --no-cache-dir -r requirements.txt

COPY . .

CMD ["python3", "main.py"]
