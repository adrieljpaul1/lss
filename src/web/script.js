// WebSocket connection
const ws = new WebSocket('ws://5.196.78.28:1880');

// Store sector data
let sectorData = {};

// WebSocket event handlers
ws.onopen = () => {
    console.log('Connected to WebSocket server');
};

ws.onmessage = (event) => {
    try {
        const data = JSON.parse(event.data);
        updateSectorData(data);
    } catch (error) {
        console.error('Error processing message:', error);
    }
};

ws.onerror = (error) => {
    console.error('WebSocket error:', error);
    console.error('WebSocket readyState:', ws.readyState);
};

// Update sector data and UI
function updateSectorData(data) {
    const { sectorId, moisture, motor } = data;
    
    // Store the data
    sectorData[sectorId] = {
        moisture: moisture,
        motor: motor
    };
    
    // Update UI
    updateProgressBars(sectorId, moisture);
    updateMotorState(sectorId, motor);
}

// Update progress bars for a sector
function updateProgressBars(sectorId, moistureArray) {
    if (!Array.isArray(moistureArray) || moistureArray.length !== 4) {
        console.error('Invalid moisture data for sector', sectorId);
        return;
    }

    // Update individual sensor bars
    moistureArray.forEach((value, index) => {
        const sensorBar = document.querySelector(`#sector${sectorId}-sensor${index + 1}`);
        if (sensorBar) {
            sensorBar.style.width = `${value}%`;
            sensorBar.textContent = `${value.toFixed(1)}%`;
        }
    });

    // Update average
    const avg = moistureArray.reduce((a, b) => a + b, 0) / moistureArray.length;
    const avgBar = document.querySelector(`#sector${sectorId}-avg`);
    const avgLabel = document.querySelector(`#sector${sectorId}-avg-label`);

    if (avgBar && avgLabel) {
        avgBar.style.width = `${avg}%`;
        avgLabel.textContent = `Avg: ${avg.toFixed(1)}%`;
    }
}

// Update motor state UI
function updateMotorState(sectorId, state) {
    const toggle = document.getElementById(`toggle${sectorId}`);
    if (toggle) {
        if (state === 1) {
            toggle.classList.add('on');
        } else {
            toggle.classList.remove('on');
        }
    }
}

// Handle motor toggle
function toggleState(sectorId) {
    const toggle = document.getElementById(`toggle${sectorId}`);
    const isOn = toggle.classList.toggle('on');
    const state = isOn ? 1 : 0;

    // Get current moisture values or default to zeros
    const currentData = sectorData[sectorId] || { moisture: [0, 0, 0, 0] };

    // Send update to server
    if (ws.readyState === WebSocket.OPEN) {
        const payload = {
            sectorId: sectorId,
            moisture: currentData.moisture,
            motor: state
        };
        ws.send(JSON.stringify(payload));
    }
}