import "./css/console.css"
import Button from '@mui/material/Button'
import { useState } from "react"

//custom console component, current has hardcoded values


function Console() {

    const [error, setError] = useState("")
    function handleClick() {

        if (error === "") {
            setError("there is an error")
        } else {
            setError("")
        } 
    }

    return (
        <>
            <button onClick={handleClick}> temp </button>
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
                        <Button variant="contained" className="clearButton">CLEAR</Button>
                </div>
            </div>
        </>
    )
}

export default Console