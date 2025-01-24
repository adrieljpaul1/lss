const mqtt = require('mqtt');

// Connect to the MQTT broker (replace with your broker URL if needed)
const topics = [];
const client = mqtt.connect('mqtt://5.196.78.28');

// Create an array of topic names (sector1 to sector7)
for(let i = 1; i <= 7; i++) {
    topics.push(`sector${i}`);
}

console.log('Topics:', topics);

// When connected to the broker
client.on('connect', () => {
  console.log('Connected to MQTT broker');

  // Loop through all topics and publish random moisture data
  for (let i = 0; i < topics.length; i++) {
    const random = [
      Math.floor(Math.random() * 100),
      Math.floor(Math.random() * 100),
      Math.floor(Math.random() * 100),
      Math.floor(Math.random() * 100)
    ];

    const message = {
      moisture: random
    };

    console.log(`Publishing message to ${topics[i]}:`, message);

    // Publish the message to the current topic
    client.publish(topics[i], JSON.stringify(message), (err) => {
      if (err) {
        console.log('Error publishing message:', err);
      } else {
        console.log('Message published to', topics[i]);
      }
    });
  }

  // Close the connection after publishing all messages
  client.end();
});
