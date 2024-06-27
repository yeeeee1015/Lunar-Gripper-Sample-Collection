import React, { useState, useEffect } from 'react';

const fetchSensorData = () => { // Simulate a function that fetches sensor data
  return Math.random() * 100; // Replace this with real sensor data fetching logic
};

const SensorData = () => {


  const [sensorData, setSensorData] = useState(0);

  useEffect(() => {


    const updateSensorData = () => {  // Function to update sensor data
      setSensorData(fetchSensorData());
    };
    
    const interval = setInterval(updateSensorData, 1000); // Update sensor data every second
    return () => clearInterval(interval);   // Cleanup interval on component unmount


  }, []);


  return (
    <div>
      <h1>Sensor Data</h1>
      <p>{sensorData.toFixed(2)}</p>
    </div>
  );
};

export default SensorData;