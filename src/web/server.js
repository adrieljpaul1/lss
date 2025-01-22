const WebSocket = require('ws');
const mqtt = require('mqtt');

const mqttBroker = 'mqtt://5.196.78.28:1883';

const client = mqtt.connect(mqttBroker);

const ws = new WebSocket.Server({ port: 8080 });

// MQTT client connection and subscription
client.on('connect', () => {
    console.log('Connected to MQTT broker!');
    client.subscribe(subscriber, (err) => {
        if (err) {
            console.error('Subscription error:', err);
        } else {
            console.log(`Subscribed to topic: ${subscriber}`);
        }
    });
});

// MQTT error handling
client.on('error', (err) => {
    console.error('MQTT error:', err);
});

wss.on('connection', (ws) => {
    console.log('Client connected to WebSocket');

    // Handle received MQTT messages and update the values
    client.on('message', (topic, message) => {
        const msg = message.toString();
        console.log(`Received message: "${msg}" from topic: "${topic}"`);

        try {
            const json_message = JSON.parse(msg);
            const moistureArray = json_message.moisture;
            const intArray = moistureArray.map(num => parseInt(num, 10));
            console.log('Converted moisture values:', intArray);

            // Send updated moisture values to the WebSocket client
            ws.send(JSON.stringify({
                a: intArray[0],
                b: intArray[1],
                c: intArray[2],
                d: intArray[3]
            }));
        } catch (error) {
            console.error('Error parsing message:', error);
        }
    });

    // WebSocket client disconnection handling
    ws.on('close', () => {
        console.log('Client disconnected');
    });
});

// Publish MQTT message with sector data and motor state
function publishMQTT(sectorId, payload) {
    const topic = `sector${sectorId}`;
    const message = JSON.stringify(payload);

    client.publish(topic, message, (err) => {
        if (err) {
            console.error(`Error publishing to ${topic}:`, err);
        } else {
            console.log(`Data sent to ${topic}`);
        }
    });
}

// Update progress bars and labels for moisture data
function updateProgressBar(sectorId, moistureData) {
    // Update individual sensor progress bars and labels
    for (let i = 1; i <= 4; i++) {
        const sensorBar = document.querySelector(`#sector${sectorId}-sensor${i}`);
        if (sensorBar) {
            const moistureValue = moistureData[i - 1];
            sensorBar.textContent = `${moistureValue.toFixed(1)}%`;
        }
    }
}
