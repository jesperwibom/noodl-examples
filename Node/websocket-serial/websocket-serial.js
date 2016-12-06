var WebSocketServer = require('ws').Server,
    SerialPort = require("serialport");

var port = "/dev/cu.usbmodem1421";
//var port = "COM7";
var isOpen = false;

var wss = new WebSocketServer({
    port: 1987
});
var serialPort = new SerialPort(port, {
    baudrate: 9600
});

console.log("Opening ", port);


serialPort.on("open", function() {
    console.log("Port", port, "opened");
    isOpen = true;
});

serialPort.on("error", function() {
    console.log(port, 'error');
});

serialPort.on("close", function() {
    console.log("Port", port, "closed");
    isOpen = false;
});

var accumulatedData = '';

serialPort.on('data', function(data) {
    accumulatedData += '' + data;

    var commands = accumulatedData.split(';');

    if(commands.length > 1) {
      console.log("Arduino log: " + commands[0]);
        for(var i = 0; i < commands.length - 1; i++) {
            var command = commands[i];
            wss.clients.forEach(function each(client) {
                console.log("writing to Noodl");
                client.send(JSON.stringify({data: command}));
            });
        }
        accumulatedData = commands[commands.length - 1];
    }

});


wss.on('connection', function(ws) {
    console.log("New client connected");
    newClient(ws);
});

function newClient(ws) {

    ws.on('message', function(message) {

        var msg = JSON.parse(message);
        var toArduino = msg.command+":"+msg.argument+";";
        console.log("Noodl command: " + toArduino);

        if(serialPort && isOpen) {
            console.log("writing to Arduino:")
            serialPort.write(toArduino);
        }
    });

    ws.on("close", function() {
        console.log("Client disconnected");
    });
}
