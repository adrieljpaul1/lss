//console.log("Enter a username");
function getUserInput() {
    const username = document.getElementById('username').value;
    switch(username){
        case 'admin':
            alert('welcome admin');
            break;
        case `gamaliyel`:
            alert('welcome gamaliyel');
            break;
        default:
            console.log('Invalid username');
    }
    const password = document.getElementById('password').value;
    switch(password){
        case 'admin':
            alert('welcome admin');
            break;
        default:
            console.log('Invalid password');
    }
}