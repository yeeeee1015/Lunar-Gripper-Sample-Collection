import "./css/console.css"
import Button from '@mui/material/Button'
import { useState } from "react"

//custom console component, current has hardcoded values



function Console() {
    const [status, setStatus] = useState([])
    function handleClick() {
        console.log('added')
        setStatus(t => [...t , "new error"])
        console.log(status)
    }

    function clearConsole() {
        console.log('cleared')
        setStatus(t => [])
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
                          <ol>
                            {status.map((stat, index) => (
                                <li key={index}>
                                    <span className="statusText">{stat}</span>
                                </li>
                            ))}
                          </ol>
                      </div>
                  </div>
              </div>
              <div className="clearButtonContainer">
                      <Button variant="contained" className="clearButton" onClick={clearConsole}>CLEAR</Button>
              </div>
          </div>
        </>
    )
}

export default Console