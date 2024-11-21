import requests

# The URL of the server where you want to send the data
server_url = 'http://192.168.1.100:5000/receive_data'  # Replace with server IP

# Data to send
data = {'message': 'Hello from the client!'}

# Send the data as a POST request with JSON
response = requests.post(server_url, json=data)

# Print the server's response
print(response.text)


import serial
import time

# Set the serial port (adjust if necessary)
ser = serial.Serial('/dev/ttyUSB0', 9600)  # Change the port based on your system
time.sleep(2)  # Wait for the connection to establish

# Main loop to read data from Arduino
while True:
    if ser.in_waiting > 0:
        # Read a line from the serial port
        data = ser.readline().decode('utf-8').strip()
        
        # Check if data is not empty
        if data:
            print(f"Received data: {data}")
            # Optionally, parse the data if necessary
            # For example, you can extract temperature and humidity values
            if "Temperature:" in data and "Humidity:" in data:
                temp_data = data.split(", ")
                temperature = temp_data[0].split(": ")[1]
                humidity = temp_data[1].split(": ")[1]
                
                print(f"Temperature: {temperature} °C, Humidity: {humidity} %")
    time.sleep(1)  # Wait before checking again
