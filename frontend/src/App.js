//This is the

import Button from '@mui/material/Button'
import myImage from './assets/gripperNoBackgroundResized.png'
import "./css/app.css"
import "./css/console.css"
import { useState, useEffect} from "react"
import * as React from 'react';
import Table from '@mui/material/Table';
import TableBody from '@mui/material/TableBody';
import TableCell from '@mui/material/TableCell';
import TableContainer from '@mui/material/TableContainer';
import TableHead from '@mui/material/TableHead';
import TableRow from '@mui/material/TableRow';
import Paper from '@mui/material/Paper';
import Slider from '@mui/material/Slider';

const io = require("socket.io-client")
const socket = io("http://localhost:3001")

const buttonSize = '40px'
const startColor = '#00b061'
const releaseColor = '#f7a305'
const killSwitchColor = '#d62206'
const pauseColor = '#A9A9A9'
const resetColor = '#7F00FF'
let inputData = {
  sensorNumber : -1,
  sensorData: null
}

let boolPause = false
let armDist = 0
let gripperPercent = 0
let currStatus = null

socket.on("connection", () => {
  console.log("this is the client, just connected")
})


const pauseButtonStyles = {
  fontSize: buttonSize,
  backgroundColor: pauseColor
}

const resetButtonStyles = {
  fontSize: buttonSize,
  backgroundColor: resetColor
}

const buttonStartStyles = {
  fontSize: buttonSize,
  backgroundColor: startColor
}
const buttonReleaseStyles = {
  fontSize: buttonSize,
  backgroundColor: releaseColor
}
const buttonKillSwitchStyles = {
  fontSize: '60px',
  backgroundColor: killSwitchColor,
  width: '100%'
}
const container1Styles = {
  display: 'flex', 
  flexDirection: 'row', 
  justifyContent: 'space-evenly', 
  flex: 1
}
const buttonContainerStyle = {
  display: 'flex', 
  flexDirection: 'column', 
  justifyContent: 'space-evenly', 
  alignItems: 'stretch'
}

const consoleContainerStyle = {
  height: '100%',
  display: 'flex',
  justifyContent: "center",
  alignItems: "center"
}
const outerDivStyle = {
  height: '100%',
  display: "flex",
  flexDirection: "column"
}
const headerStyle = {
  height: '100px',
  width: '100%',
  borderRadius: '0px',
  justifyContent: 'start'
}
const imageStyle = {
  marginLeft: '20px'
}

function App() {

  const [status, setStatus] = useState([])

    function addStatus(initStatus) {

      if (status.length >= 25) {
        setStatus(t => [initStatus])
      } else {
        setStatus(t => [...t , initStatus])
      }

    }
    function handleClick(initStatus, sensNum, sensData) {
     
      addStatus(initStatus)
      inputData.sensorNumber = sensNum
      inputData.sensorData = sensData
      socket.emit("uiData", inputData)
      inputData = {
        sensorNumber : -1,
        sensorData: null
      }

    }

    function startButton() {
      handleClick("Starting Collection", 1, null)
    }
    function releaseButton() {
      handleClick("Releasing", 2, null)
    }
    function killSwitchButton() {
      handleClick("Killing power", 3, null)
    }
    function pauseButton() {
      if (boolPause) {
        setButtonText("Pause")
        handleClick("Unpaused",4,null)
      } else {
        setButtonText("Unpause")
        handleClick("Paused",4,null)
      }
      boolPause = !boolPause
    }
    function resetButton() {
      handleClick("Resetting",5,null)
    }
    
    const armSliderChange = (event, newValue) => {
      setArmSliderValue(newValue)
      armDist = newValue
    }

    const gripperSliderChange = (event, newValue) => {
      setGripperSliderValue(newValue)
      gripperPercent = newValue
      
    }

    function armSliderGoButton() {
      handleClick("Arm is moving to " + armDist +" cm",6,armDist)
    }

    function gripperSliderGoButton() {
      handleClick("Gripper is open to " + gripperPercent +"°",7,gripperPercent)
    }

    function clearConsole() {
        setStatus(t => [])
    }

    //tfmini: TFMini,
    // prox: Prox,
    // vl53l0x: VL53L0X,
    // lim1: limit1,
    // lim2: limit2,
    // lim3: limit3,
    // press1: Pressure1,
    // press2: Pressure2,
    // press3: Pressure3,
    // dcEnc: dcEncoder,
    // servoEnc: servoEncoder,
    // stat: currStatus 
  const [buttonText, setButtonText] = useState("Pause")
  const [armSliderValue, setArmSliderValue] = useState(0);
  const [gripperSliderValue, setGripperSliderValue] = useState(0);
  const [tfMini, setTfMini] = useState(0);
  const [prox, setProx] = useState(0);
  const [vl53l0x, setVl53l0x] = useState(0);
  const [limitSwitch1, setLimitSwitch1] = useState(0);
  const [limitSwitch2, setLimitSwitch2] = useState(0);
  const [limitSwitch3, setLimitSwitch3] = useState(0);
  const [pressure1, setPressure1] = useState(0);
  const [pressure2, setPressure2] = useState(0);
  const [pressure3, setPressure3] = useState(0);
  const [servoEncoder, setServoEncoder] = useState(0);
  const [dcEncoder, setDcEncoder] = useState(0);

  

  useEffect(() => {
    socket.on('sendingData', (data)=> {
      setTfMini(data.tfmini)
      setProx(data.prox)
      setVl53l0x(data.vl53l0x)
      setLimitSwitch1(data.lim1)
      setLimitSwitch2(data.lim2)
      setLimitSwitch3(data.lim3)
      setPressure1(data.press1)
      setPressure2(data.press2)
      setPressure3(data.press3)
      setDcEncoder(data.dcEnc)
      setServoEncoder(data.servoEnc)
      setStatus((prevStatus) => {
        if (currStatus !== data.stat) {
          currStatus = data.stat
          if (prevStatus.length >= 25) {
            return [data.stat];
          } else {
            return [...prevStatus, data.stat];
          }
        }
        return prevStatus;
      });

      
    })
    return () => socket.off('sendingData');   // Cleanup interval on component unmount
  }, []);

  function createData(name, value, unit) {
  return { name, value, unit};
}

const rows = [
  createData("TFMini", tfMini, "cm"),
  createData("Proximity Sensor", prox, "binary"),
  createData("VL53L0X", vl53l0x, "mm"),
  createData("DC Motor Encoder", servoEncoder, "unit?"),
  createData("Servo Motor Encoder", dcEncoder, "unit?"),
  createData("Limit Switch 1", limitSwitch1, "binary"),
  createData("Limit Switch 2", limitSwitch2, "binary"),
  createData("Limit Switch 3", limitSwitch3, "binary"),
  createData("Pressure Sensor 1", pressure1, "unit?"),
  createData("Pressure Sensor 2", pressure2, "unit?"),
  createData("Pressure Sensor 3", pressure3, "unit?"),

];

  return (
    <div className="outerDiv" style={ outerDivStyle}>
      <div className="header" style={headerStyle}>
        <img className="image" src={myImage} style={imageStyle} alt='gripper'/>
        <p className="headerText" >Lunar Gripper</p>
      </div>
      <div className="container1" style={container1Styles}>
        <div className="buttonContainer" style={buttonContainerStyle}>
              <Button onClick={startButton} className='buttonStart' variant="contained" style={buttonStartStyles}>Start Collection</Button>
              <Button onClick={releaseButton} className='button' variant="contained" style={buttonReleaseStyles}>Release</Button>
              <Button onClick={pauseButton} className='button' variant="contained" style={pauseButtonStyles}>{buttonText}</Button>
              <Button onClick={resetButton} className='button' variant="contained" style={resetButtonStyles}>Reset</Button>
              <div className="sliderContainer">
                <p className="sliderTitle">Open Gripper to X °</p>
                <div className='constraint'>
                  <Slider min={-90} max={90} value={gripperSliderValue} onChange={gripperSliderChange}/>
                </div>
                <div className='buttonNValue'>
                  <p className="sliderText">{gripperSliderValue + '°'}</p>
                  <Button variant="contained" onClick={gripperSliderGoButton}sx={{fontSize: '15px', padding: '3px'}}>GO</Button>
                </div>
                  
              </div>
              <div className="sliderContainer">
                <p className="sliderTitle">Move Arm to X Distance</p>
                  <div className="constraint">
                    <Slider min={-45} max={45.72} value={armSliderValue} onChange={armSliderChange}/>
                  </div>
                  <div className='buttonNValue'>
                    <p className="sliderText">{armSliderValue + ' cm'}</p>
                    <Button variant="contained" onClick={armSliderGoButton} sx={{fontSize: '15px', padding: '3px'}}>GO</Button>
                  </div>
                  
              </div>

        </div>
        <div className="middleContainer">
        <Button onClick={killSwitchButton} className='button' variant="contained" style={buttonKillSwitchStyles}>Kill Switch</Button>
          <div className='tableContainer'>
            <TableContainer component={Paper}>
              <Table sx={{ minWidth: 350, minHeight: 500}} aria-label="simple table">
                <TableHead>
                  <TableRow>
                    <TableCell>Sensors</TableCell>
                    <TableCell align="right">Value</TableCell>
                    <TableCell align="right">Unit</TableCell>
                  </TableRow>
                </TableHead>
                <TableBody>
                  {rows.map((row) => (
                    <TableRow
                      key={row.name}
                      sx={{ '&:last-child td, &:last-child th': { border: 0 } }}
                    >
                      <TableCell component="th" scope="row">
                        {row.name}
                      </TableCell>
                      <TableCell align="right">{row.value}</TableCell>
                      <TableCell align="right">{row.unit}</TableCell>
                    </TableRow>
                  ))}
                </TableBody>
              </Table>
            </TableContainer>
          </div>
        </div>
        <div className="consoleContainer" style={consoleContainerStyle}>
        <div className="container">
              <div className="consoleDiv">
                  <div className="header">
                      <p className="consoleHeaderText">Console</p>
                  </div>
                  <div className="body">
                      <div className="buffer">
                          <ul>
                            {status.map((stat, index) => (
                                <li key={index}>
                                    <span className="statusText">{stat}</span>
                                </li>
                            ))}
                          </ul>
                      </div>
                  </div>
              </div>
              <div className="clearButtonContainer">
                      <Button variant="contained" className="clearButton" onClick={clearConsole}>CLEAR</Button>
              </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
