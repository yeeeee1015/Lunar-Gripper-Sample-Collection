const { SerialPort } = require("serialport") 

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
    port.close(()=>{
      console.log("port is closed");
    })
  })
  