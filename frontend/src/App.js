
import Stack from '@mui/material/Stack'
import Container from '@mui/material/Container'
import ButtonGroup from '@mui/material/ButtonGroup'
import Button from '@mui/material/Button'


function App() {
  return (
    <Stack spacing={2} direction="row " maxHeight={false}>
      <Container>
        <div style={{display: 'flex', alignItems: 'center', height: '100vh'}}>
          <ButtonGroup orientation="vertical" >
            <Button style={{fontSize: '40px'}}>Start</Button>
            <Button style={{fontSize: '40px'}}>Release</Button>
            <Button style={{fontSize: '40px'}}>Stop</Button>
            <Button style={{fontSize: '40px'}}>Pause</Button>
            <Button style={{fontSize: '40px'}}>Kill Switch</Button>
            <Button style={{fontSize: '40px'}}>Reset</Button>
          </ButtonGroup>
        </div>
        
      </Container>
      <Container>
      <p> item two</p>

      </Container>
      <Container>
      <p> item three</p>

      </Container>
    </Stack>
  );
}

export default App;
