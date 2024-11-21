from flask import Flask, request

app = Flask(__name__)

@app.route('/receive_data', methods=['POST'])
def receive_data():
    data = request.json  # Get the JSON data sent by the client
    print(f"Received data: {data}")
    return "Data received successfully!", 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)  # Listen on all interfaces, port 5000
