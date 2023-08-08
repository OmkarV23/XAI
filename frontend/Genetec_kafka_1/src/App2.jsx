import React, { useEffect, useState } from 'react';
import { Buffer } from 'buffer';

import socketIOClient from 'socket.io-client';
import './App.css';
import { WebSocketServer } from 'ws';

const App = () => {
  const socket = new WebSocket('ws://54.183.246.209:5173');
  const [imageData, setImageData] = useState([]);


  // 
  useEffect(() => {
    console.log("Hello")
    localStorage.removeItem('imageData');
    socket.addEventListener('message', ({ data }) => {
      const packet = JSON.parse(data);
      console.log(packet); // printing the values in the console (for debugging)

      // const decodedImage = Buffer.from(packet.imageString, 'base64').toString('base64');

      // Creating a new image Array where the information about the frames are stored
      const updatedImageData = [
        {
          cls: packet.cls,
          confidence: packet.confidence,
          timestamp: packet.timestamp,
          image: decodedImage,
        },
        // ...imageData.splice(0,2)
      ];
      setImageData(updatedImageData);

      // Store the updated image data in local storage
      localStorage.setItem('imageData', JSON.stringify(updatedImageData));
    });

    // Load previous image data from local storage on initial mount
    // const storedImageData = localStorage.getItem('imageData');
    // if (storedImageData) {
    //   setImageData(JSON.parse(storedImageData));
    // }
  }, [imageData]);

  return (
    <div className="app">
      
      {imageData.map((data, index) => (
        <div className="image-container" key={index}>
          <p>Class: {data.cls}</p>
          <p>Confidence: {data.confidence}</p>
          <p>Timestamp: {data.timestamp}</p>
          <img
            className={data.confidence > 0.3 ? 'highlighted-image' : ''}
            src={`data:image/jpeg;base64, ${data.image}`}
            alt={`Image ${index}`}
          />
        </div>
      ))}
    </div>
  );
};

export default App;
