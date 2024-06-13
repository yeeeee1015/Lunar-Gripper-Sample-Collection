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

const buttonStyles = {
  fontSize: '40px',
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

  const [error, setError] = useState("")
  function addError() {

    setError(error + " error" + "\n")
      
  }
  function clearError() {
    setError("")
  }

  const [sensorData1, setSensor1Data] = useState(0);
  const [sensorData2, setSensor2Data] = useState(0);
  const [sensorData3, setSensor3Data] = useState(0);
  const [sensorData4, setSensor4Data] = useState(0);
  const fetchSensorData = () => { // Simulate a function that fetches sensor data
    return (Math.random() * 100).toFixed(3); // Replace this with real sensor data fetching logic
  };
  

  useEffect(() => {
    const updateSensorData = () => {  // Function to update sensor data
      setSensor1Data(fetchSensorData());
      setSensor2Data(fetchSensorData());
      setSensor3Data(fetchSensorData());
      setSensor4Data(fetchSensorData());
    };
    const interval = setInterval(updateSensorData, 500); // Update sensor data every second
    return () => clearInterval(interval);   // Cleanup interval on component unmount
  }, []);


  function createData(name, value, unit) {
  return { name, value, unit};
}

const rows = [
  createData("Sensor 1", sensorData1, "N"),
  createData("Sensor 2", sensorData2, "Feet"),
  createData("Sensor 3", sensorData3, "Meters"),
  createData("Sensor 4", sensorData4, "PSI"),
  
];


  return (
    <div className="outerDiv" style={ outerDivStyle}>
      <div className="header" style={headerStyle}>
        <img className="image" src={myImage} style={imageStyle} alt='gripper'/>
        <p className="headerText" >Lunar Gripper</p>
      </div>
      <div className="container1" style={container1Styles}>
        <div className="buttonContainer" style={buttonContainerStyle}>
              <Button onClick={addError} className='button' variant="contained" style={buttonStyles}>Start</Button>
              <Button onClick={addError} className='button' variant="contained" style={buttonStyles}>Release</Button>
              <Button onClick={addError} className='button' variant="contained" style={buttonStyles}>Stop</Button>
              <Button onClick={addError} className='button' variant="contained" style={buttonStyles}>Pause</Button>
              <Button onClick={addError} className='button' variant="contained" style={buttonStyles}>Kill Switch</Button>
              <Button onClick={addError} className='button' variant="contained" style={buttonStyles}>Reset</Button>
        </div>
        <div className="tableContainer" style={tableContainerStyle}>
          <TableContainer component={Paper}>
            <Table sx={{ minWidth: 350, minHeight: 600}} aria-label="simple table">
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
                          <p className="errors">{error}</p>
                      </div>
                  </div>
              </div>
              <div className="clearButtonContainer">
                      <Button onClick={clearError} variant="contained" className="clearButton">CLEAR</Button>
              </div>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
