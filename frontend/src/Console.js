import "./css/console.css"


function Console() {
    return (
        <>
            <div className="container">
                <div className="consoleContainer">
                        <div className="header">
                            <p className="consoleHeader">Console</p>
                        </div>
                        <div className="body">
                            <p className="errors">temp values</p>
                        </div>
                    </div>
                    <div className="clearButtonContainer">
                        <button className="clearButton">CLEAR</button>
                </div>
            </div>
        </>
    )
}

export default Console