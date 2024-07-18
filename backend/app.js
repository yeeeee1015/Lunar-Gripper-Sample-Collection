const { SerialPort } = require("serialport") 

const express = require('express');
const { createServer } = require('node:http');
const { join } = require('node:path');
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

const port = new SerialPort({
  //path: "/dev/ttyACM0",
  path: "COM4",
  autoOpen: false,
  baudRate: 115200
})

port.open(() => {
  console.log("port is opened")
  
})

port.write("open", ()=> {
  console.log("writing to port")
  // port.close(()=>{
  //   console.log("port is closed");
  // })
})



io.on('connection', (socket) => {
  console.log('a user connected');

  socket.on('sendingStatus', (_currStatus) => {
    console.log(_currStatus)
    currStatus = _currStatus
    
  })

});



server.listen(portNumber, () => {
  console.log('server running at http://localhost:3001');
});

let TFMini = null // index 0
let Prox = null // index 1
let Pressure = null // index 2
let VL53L0X = null // index 3
let limit = null // index 4

function deserialize(data) {
    let array = data.toString().slice(0,data.toString().length - 2).split("@") // converts to string, removes garbage characters, splits by @ into array
    TFMini = array[0]
    Prox = array[1]
    Pressure = array[2]
    VL53L0X = array[3]
    limit = array[4]
    // console.log(TFMini, Prox,Pressure,VL53L0X,limit)
}

function jsonify() {
  return {
    tfmini: TFMini,
    prox: Prox,
    pressure: Pressure,
    vl53l0x: VL53L0X,
    limitSwitch: limit
  }
}

port.on('data', (data) => {

  port.write("some data")

    deserialize(data)

    io.emit('sendingData', jsonify())

})

port.on('error', (err)=> {
  console.log("error:", err)
})