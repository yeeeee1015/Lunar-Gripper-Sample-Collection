import Box from '@mui/material/Box'
import Button from '@mui/material/Button'
import Output from "./Output"


const buttonStyles = {
  fontSize: '40px',
  // margin: '10px'
}

const buttonContainerStyle = {
  display: 'flex', 
  flexDirection: 'column', 
  justifyContent: 'space-evenly', 
  alignItems: 'stretch'
}

function App() {
  return (
    <div className="container1" style={{display: 'flex', flexDirection: 'row', justifyContent: 'space-evenly', height: '100%'}}>
      <div className="buttonContainer" style={buttonContainerStyle}>
            <Button variant="contained" style={buttonStyles}>Start</Button>
            <Button variant="contained" style={buttonStyles}>Release</Button>
            <Button variant="contained" style={buttonStyles}>Stop</Button>
            <Button variant="contained" style={buttonStyles}>Pause</Button>
            <Button variant="contained" style={buttonStyles}>Kill Switch</Button>
            <Button variant="contained" style={buttonStyles}>Reset</Button>
      </div>
      <div className="tableContainer" style={buttonContainerStyle}>
      <Output></Output>

      </div>
      <Box>
      <p> item three</p>

      </Box>
    </div>
  );
}

export default App;
