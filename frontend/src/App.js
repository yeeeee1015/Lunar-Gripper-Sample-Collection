import Button from '@mui/material/Button'
import Output from "./Output"
import Console from "./Console"
import myImage from './assets/gripperNoBackgroundResized.png'
import "./css/app.css"


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

// const headerTextStyle = {
//   fontSize: "40px",
//   color: 'white',
// }

function App() {
  return (
    <div className="outerDiv" style={ outerDivStyle}>
      <div className="header" style={headerStyle}>
        <img className="image" src={myImage} style={imageStyle} alt='gripper'/>
        <p className="headerText" >Lunar Gripper</p>
      </div>
      <div className="container1" style={container1Styles}>
        <div className="buttonContainer" style={buttonContainerStyle}>
              <Button className='button' variant="contained" style={buttonStyles}>Start</Button>
              <Button className='button' variant="contained" style={buttonStyles}>Release</Button>
              <Button className='button' variant="contained" style={buttonStyles}>Stop</Button>
              <Button className='button' variant="contained" style={buttonStyles}>Pause</Button>
              <Button className='button' variant="contained" style={buttonStyles}>Kill Switch</Button>
              <Button className='button' variant="contained" style={buttonStyles}>Reset</Button>
        </div>
        <div className="tableContainer" style={tableContainerStyle}>
          <Output/>
        </div>
        <div className="consoleContainer" style={consoleContainerStyle}>
          <Console/>
        </div>
      </div>
    </div>
  );
}

export default App;
