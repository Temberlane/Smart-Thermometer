from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/send', methods=['POST'])
def receive_message():
    data = request.json  # Get JSON data from the request
    message = data.get('message')
    if message:
        print(f"Received message: {message}")
        return jsonify({"status": "success", "message": "Message received!"}), 200
    else:
        return jsonify({"status": "error", "message": "No message received"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5500)  # Accessible on the local network
