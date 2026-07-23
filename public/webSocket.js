const socket = new WebSocket('ws://localhost:8080');

//פונקציה של חיבור ראשוני
socket.onopen = () => {
    console.log('Socket opened');


    //פונקציה של שליחת מידע ללקוח השני
    socket.send(JSON.stringify({id: "web"}));

    //אירוע של קבלת הודעה
    socket.onmessage = event => {
        console.log(event.data);
        let message = JSON.parse(event.data);
        //פה לשים את ההודעה בHTML
        document.getElementById('receivedMessage').innerHTML = message.message;
    }

}

function sendMessage(message) {
    socket.send(JSON.stringify(
        {
            from: "web",
            to: "esp",
            message: message
        }
    ));

}

function backToIdle(message) {
    socket.send(JSON.stringify(
        {
            from: "web",
            to: "esp",
            state: 102,
            message: message
        }
    ));

}
let btn = document.getElementById("rst_btn");
btn.addEventListener("click", ()=>{
    backToIdle("reset button")
});
