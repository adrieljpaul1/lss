// Global variables
let topics = [];

// Assign values to topics dynamically
for (let i = 1; i <= 7; i++) {
    for (let j = 1; j <= 4; j++) {
        const topic = `sector${i}-sensor${j}`;
        console.log(topic);
        topics.push(topic);
    }
}

// Connect to WebSocket server
const ws = new WebSocket('ws://localhost:8080');

ws.onopen = () => {
    console.log('Connected to WebSocket server');
};

ws.onmessage = (event) => {
    const data = JSON.parse(event.data);

    // Update displayed values dynamically
    Object.keys(data).forEach((sector) => {
        data[sector].forEach((moistureValue, sensorIndex) => {
            // Update sensor value display
            const valueElement = document.getElementById(`${sector}-sensor${sensorIndex + 1}-value`);
            if (valueElement) {
                valueElement.textContent = moistureValue + '%';
            }

            // Update progress bar
            const progressBar = document.getElementById(`${sector}-sensor${sensorIndex + 1}-progress`);
            if (progressBar) {
                progressBar.style.width = moistureValue + '%';
                progressBar.textContent = moistureValue + '%';
            }
        });
    });
};

ws.onclose = () => {
    console.log('Disconnected from WebSocket server');
};

// Function to dynamically create sensor HTML elements
function createSensorElements() {
    const container = document.getElementById('sensor-container');

    for (let i = 1; i <= 7; i++) {
        const sectorDiv = document.createElement('div');
        sectorDiv.className = 'sector';
        sectorDiv.id = `sector${i}`;
        sectorDiv.innerHTML = `<h2>Sector ${i}</h2>`;

        for (let j = 1; j <= 4; j++) {
            const sensorDiv = document.createElement('div');
            sensorDiv.className = 'sensor';
            sensorDiv.id = `sector${i}-sensor${j}`;
            sensorDiv.innerHTML = `
                <p>Sensor ${j}: <span id="sector${i}-sensor${j}-value">0%</span></p>
                <div class="progress-bar-container">
                    <div id="sector${i}-sensor${j}-progress" class="progress-bar"></div>
                </div>
            `;
            sectorDiv.appendChild(sensorDiv);
        }

        container.appendChild(sectorDiv);
    }
}

// Call the function to create sensor elements on page load
window.onload = createSensorElements;
