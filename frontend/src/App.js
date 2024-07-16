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

const io = require("socket.io-client")
const socket = io("http://localhost:3001")

let buttonSize = '20px'
let startColor = '#00b061'
let releaseColor = '#f7a305'
let killSwitchColor = '#d62206'

let boolPause = false
let armUpDist = 0
let armDownDist = 0

socket.on("connection", () => {
  console.log("this is the client, just connected")
})


const buttonStyles = {
  fontSize: buttonSize,
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
  fontSize: buttonSize,
  backgroundColor: killSwitchColor
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
const tableContainerStyle = {
  display: "flex",
  justifyContent: 'center',
  alignItems: 'center',
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
    function handleClick(err) {
      if (status.length >= 25) {
        setStatus(t => [err])
      } else {
        setStatus(t => [...t , err])
      }
      socket.emit("sendingStatus", err)
      
    }

    function startButton() {
      handleClick("starting")
    }
    function releaseButton() {
      handleClick("releasing")
    }
    function killSwitchButton() {
      handleClick("killing power")
    }
    function pauseButton() {
      if (boolPause) {
        handleClick("unpaused")
      } else {
        handleClick("paused")
      }
      boolPause = !boolPause
    }
    function resetButton() {
      handleClick("resetting")
    }
    function openClawButton() {
      handleClick("changing claw position")
    }
    function armUpButton() {
      handleClick("moving arm up " + armUpDist + "cm")
    }
    function armDownButton() {
      handleClick("moving arm down " + armDownDist + "cm")
    }

    function clearConsole() {
        console.log('cleared')
        setStatus(t => [])
    }

  const [tfMini, setTfMini] = useState(0);
  const [prox, setProx] = useState(0);
  const [pressure, setPressure] = useState(0);
  const [vl53l0x, setVl53l0x] = useState(0);
  const [limitSwitch1, setLimitSwitch1] = useState(0);

  useEffect(() => {
    socket.on('sendingData', (data)=> {
      setTfMini(data.tfmini)
      setProx(data.prox)
      setPressure(data.pressure)
      setVl53l0x(data.vl53l0x)
      setLimitSwitch1(data.limitSwitch)
    })
    return () => socket.off('sendingData');   // Cleanup interval on component unmount
  }, []);

  function createData(name, value, unit) {
  return { name, value, unit};
}

const rows = [
  createData("TFMini", tfMini, "cm"),
  createData("Proximity Sensor", prox, "binary"),
  createData("Pressure Sensor", pressure, "unit?"),
  createData("VL53L0X", vl53l0x, "mm"),
  createData("Limit Switch 1", limitSwitch1, "binary"),
];

  return (
    <div className="outerDiv" style={ outerDivStyle}>
      <div className="header" style={headerStyle}>
        <img className="image" src={myImage} style={imageStyle} alt='gripper'/>
        <p className="headerText" >Lunar Gripper</p>
      </div>
      <div className="container1" style={container1Styles}>
        <div className="buttonContainer" style={buttonContainerStyle}>
              <Button onClick={startButton} className='buttonStart' variant="contained" style={buttonStartStyles}>Start</Button>
              <Button onClick={releaseButton} className='button' variant="contained" style={buttonReleaseStyles}>Release</Button>
              <Button onClick={killSwitchButton} className='button' variant="contained" style={buttonKillSwitchStyles}>Kill Switch</Button>
              <Button onClick={pauseButton} className='button' variant="contained" style={buttonStyles}>Pause</Button>
              <Button onClick={resetButton} className='button' variant="contained" style={buttonStyles}>Reset</Button>
              <Button onClick={openClawButton} className='button' variant="contained" style={buttonStyles}>Open Claw %</Button>
              <Button onClick={armUpButton} className='button' variant="contained" style={buttonStyles}>Arm X Distance Up</Button>
              <Button onClick={armDownButton} className='button' variant="contained" style={buttonStyles}>Arm X Distance Down</Button>

        </div>
        <div className="tableContainer" style={tableContainerStyle}>
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
