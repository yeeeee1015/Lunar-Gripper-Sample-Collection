import "./css/console.css"
import Button from '@mui/material/Button'


function Console() {
    return (
        <>
            <div className="container">
                <div className="consoleContainer">
                    <div className="header">
                        <p className="consoleHeader">Console</p>
                    </div>
                    <div className="body">
                        <div className="buffer">
                            <p className="errors">temp values</p>
                        </div>
                    </div>
                </div>
                <div className="clearButtonContainer">
                        <Button variant="contained" className="clearButton">CLEAR</Button>
                </div>
            </div>
        </>
    )
}

export default Console