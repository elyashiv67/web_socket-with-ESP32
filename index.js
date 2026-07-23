import { WebSocketServer} from 'ws';

const server = new WebSocketServer({port: 8080});

const clients = new Map();


server.on('connection', (socket) => {
    let clientId = null;

    //in the socket param I get the data of the client who sends me the data
    // console.log(JSON.parse(socket));
    console.log('\nConnected to the server');

    // in here I will put everything that I want to do when I have a websocket connection

    socket.on('message', (msg) => {
        // console.log("RAW" + msg.toString());

        try{
            const data = JSON.parse(msg);
            // whenever the client sends a message, this function will be activated
            console.log("Received a message from the client \n" , data);

            if(data.id && !clientId) {
                clientId = data.id;
                clients.set(clientId, socket);
                // console.log(socket);
                return;
            }

            console.log("111");
            if(data.to && data.message){
                console.log("222");
                const target = clients.get(data.to);
                // console.log(target);
                if(!target){
                    socket.send(JSON.stringify({message:"i dont know who you want to send to"}));
                    return;
                }
            if(!data.message){
                socket.send(JSON.stringify({message:"sent an empty message"}));
                return;
            }
            console.log("Message sent: " + data);
                target.send(msg.toString());

            }


        }catch (e){
            console.log(e);

        }
    });

    // socket.send(`{
    // message: "in this function the server",
    // desc: "sends a message to the client",
    // way: "in json format"
    // }`);

    socket.on('close', () => {
        console.log("Socket closed");
        // this function activates when the connection is closed for any reason
    });

});
