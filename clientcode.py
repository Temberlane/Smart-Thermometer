import requests

server_url = 'http://127.0.0.1:5500/send'  # Replace with your server's IP if on a local network

while True:
    message = input("Enter a message to send (or 'quit' to exit): ")
    if message.lower() == 'quit':
        break
    
    try:
        response = requests.post(server_url, json={'message': message})
        if response.status_code == 200:
            print(f"Server response: {response.json()['message']}")
        else:
            print(f"Failed to send message: {response.json()['message']}")
    except Exception as e:
        print(f"Error: {e}")
