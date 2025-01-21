
//global variables
let topics = [];
//assign values to topics
for(let i = 1;i<=7;i++){
    for(let j = 1;j<5;j++){
        debian = `sector${i}-sensor${j}`;
        console.log(debian);
        topics.push(debian);
    }
}
function isJsonString(str) {
    try {
        JSON.parse(str);
    } catch (e) {
        return false;
    }
    return true;
}


// Connect to WebSocket server
const ws = new WebSocket('ws://localhost:8080');

ws.onopen = () => {
    console.log('Connected to WebSocket server');
};  

ws.onmessage = (event) => {
    if(isJsonString(event.data)){
        data = JSON.parse(event.data);
    }
    else{
        console.error('Invalid JSON: ', event.data);
    }
    // const data = JSON.parse(event.data);
    // console.log(data);

    // Update the displayed values when the WebSocket message is received
    //document.getElementById('aValue').textContent = data.a;
    for(let i = 0;i<4;i++){
        document.getElementById('${i}Value').textContent = data.moisture[i];
    }
    
};


// Function to update the progress bar
function updateProgressBar(aValue) {
    for(let i = 0;i<4;i++){
        const progressBar = document.getElementById('progress-bar${i}');
        progressBar.style.width = aValue + '%';
        progressBar.textContent = aValue + '%';
    
    }
}

ws.onclose = () => {
    console.log('Disconnected from WebSocket server');
};



