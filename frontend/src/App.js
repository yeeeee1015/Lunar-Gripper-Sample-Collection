import Button from '@mui/material/Button'
import Output from "./Output"
import Console from "./Console"


const buttonStyles = {
  fontSize: '40px',
  // margin: '10px'
}

const container1Styles = {
  display: 'flex', 
  flexDirection: 'row', 
  justifyContent: 'space-evenly', 
  height: '100%'
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

function App() {
  return (
    <div className="container1" style={container1Styles}>
      <div className="buttonContainer" style={buttonContainerStyle}>
            <Button variant="contained" style={buttonStyles}>Start</Button>
            <Button variant="contained" style={buttonStyles}>Release</Button>
            <Button variant="contained" style={buttonStyles}>Stop</Button>
            <Button variant="contained" style={buttonStyles}>Pause</Button>
            <Button variant="contained" style={buttonStyles}>Kill Switch</Button>
            <Button variant="contained" style={buttonStyles}>Reset</Button>
      </div>
      <div className="tableContainer" style={tableContainerStyle}>
        <Output/>
      </div>
      <div className="consoleContainer" style={consoleContainerStyle}>
        <Console/>
      </div>
    </div>
  );
}

export default App;
