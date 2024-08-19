const { SerialPort } = require("serialport") 

const express = require('express');
const { createServer } = require('node:http');
const { Server } = require('socket.io');

const app = express();
const server = createServer(app);
const io = new Server(server, {
  cors: {
    origin: "http://localhost:3000",
    method: ["GET","POST"]
  }
});
const portNumber = 3001
let currStatus = ""
let queue = []

const port = new SerialPort({
  path: "COM8",
  baudRate: 115200,
  autoOpen: false
})

port.open(() => {
  console.log("port is opened")
  port.write('open')
})

io.on('connection', (socket) => {
  console.log('a user connected');
  queue = [];  

  socket.on('sendingStatus', (_currStatus) => {
    console.log(currStatus)
    currStatus = _currStatus
  })

  socket.on('uiData', (info) => {

    queue.push(info)
    console.log(queue)
  
  })

});



server.listen(portNumber, () => {
  console.log('server running at http://localhost:3001');
});

let TFMini = null // index 0
let Prox = null // index 1
let VL53L0X = null // index 2
let limit1 = null // index 3
let limit2 = null // index 4
let limit3 = null // index 5
let Pressure1 = null // index 6
let Pressure2 = null // index 7
let Pressure3 = null // index 8
let dcEncoder = null // index 9
let servoEncoder = null // index 10

function deserializeArduino(data) {
    let array = data.toString().slice(0,data.toString().length - 2).split("@") // converts to string, removes garbage characters, splits by @ into array
    TFMini = array[0]
    Prox = array[1]
    VL53L0X = array[2]
    limit1 = array[3]
    limit2 = array[4]
    limit3 = array[5]
    currStatus = array[6]
    // Pressure1 = array[6]
    // Pressure2 = array[7]
    // Pressure3 = array[8]
    // dcEncoder = array[9]
    // servoEncoder = array[10]
    //currStatus = array[11]
    // [TFMini, Prox, VL53L0X, limit1, limit2, limit3, Pressure1, Pressure2, Pressure3, dcEncoder, servoEncoder, currStatus] = array
    
    
    // console.log(TFMini, Prox,Pressure,VL53L0X,limit)
}

function serializeGUI() {
  let currInstruction = queue.shift()
  if (!currInstruction) {
    return "empty"
  }
  console.log(currInstruction)
  return String(currInstruction.sensorNumber) + "@" + String(currInstruction.sensorData);
}

function jsonify() {
  return {
     // TFMini = array[0]
    // Prox = array[1]
    // VL53L0X = array[2]
    // limit1 = array[3]
    // limit2 = array[4]
    // limit3 = array[5]
    // Pressure1 = array[6]
    // Pressure2 = array[7]
    // Pressure3 = array[8]
    // dcEncoder = array
    tfmini: TFMini,
    prox: Prox,
    vl53l0x: VL53L0X,
    lim1: limit1,
    lim2: limit2,
    lim3: limit3,
    press1: Pressure1,
    press2: Pressure2,
    press3: Pressure3,
    dcEnc: dcEncoder,
    servoEnc: servoEncoder,
    stat: currStatus  
  }
}



port.on('data', (data) => {

    console.log(data.toString().slice(0,data.toString().length - 2))

    deserializeArduino(data)
     
    io.emit('sendingData', jsonify())

    port.write(serializeGUI())

})

port.on('error', (err) => {
  console.log('Error: ' + err)
})
