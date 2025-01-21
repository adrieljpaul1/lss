const WebSocket = require('ws');
const mqtt = require('mqtt');
const url = 'mqtt://5.196.78.28:1883';
const subscriber = 'test/topic';

// Initialize a, b, c, d with some values (you can change these as needed)
let a ;
let b ;
let c ;
let d ;

const client = mqtt.connect(url);
let msg = '';
// Create WebSocket server
const wss = new WebSocket.Server({ port: 8080 });

wss.on('connection', (ws) => { 
    console.log('Client connected to WebSocket');
    
    // Send initial values to the client
    ws.send(JSON.stringify({ a, b, c, d }));

    // Function to handle received messages
    function receive_msg() {
        client.on('message', (topic, message) => {
            msg = message.toString();
            console.log(`Received message: "${msg}" from topic: "${topic}"`);

            try {
                let json_message = JSON.parse(msg);
                let moistureArray = json_message.moisture;
                let intArray = moistureArray.map(num => parseInt(num, 10));            
                console.log('Converted moisture values:', intArray);

                // Modify the array with values of a, b, c, and d
                a = intArray[0];
                b = intArray[1];
                c = intArray[2];
                d = intArray[3];      

                // debug message
                console.log('Modified array:', intArray);
                // Send updated values to the WebSocket client
                ws.send(JSON.stringify({ a, b, c, d }));
            } catch (error) {
                console.error('Error parsing message as JSON:', error);
            }
        });
    }

    receive_msg();

    ws.on('close', () => {
        console.log('Client disconnected');
    });
});

client.on('connect', () => {
    console.log('Connected to MQTT broker!');

    // Subscribe to a topic
    client.subscribe(subscriber, (err) => {
        if (err) {
            console.error('Subscription error:', err);
        } else {
            console.log(`Subscribed to topic: ${subscriber}`);
        }
    });
});

client.on('error', (err) => {
    console.error('MQTT error:', err);
});
