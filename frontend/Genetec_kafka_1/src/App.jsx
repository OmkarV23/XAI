import React, { useEffect, useState } from 'react';
import {Buffer} from 'buffer';

import socketIOClient from 'socket.io-client';
import './App.css';
import { WebSocketServer } from "ws";


const App = () => {
  const socket = new WebSocket("ws://13.57.244.86:5173");
  const [imageData, setImageData] = useState([]);

  useEffect(() => {
    localStorage.removeItem('imageData');
    socket.addEventListener("message", ({ data }) => {
      const packet = JSON.parse(data);
      console.log(packet) // printing the values in the console (for debugging)

      // const decodedImage = Buffer.from(packet.imageString, 'base64').toString('base64');
      //creating a new image Array where the information about the frames are stored.
      const updatedImageData = [
        ...imageData,
        {
          cls: packet.cls,
          confidence: packet.confidence,
          timestamp: packet.timestamp,
          // image: decodedImage,
        },
        // ...imageData.slice(0,2),
        
        
      ];
      
      // updating the ImageData
      setImageData(updatedImageData);
      // localStorage.setItem('imageData', JSON.stringify(updatedImageData));


      // Load previous image data from local storage on initial mount


    });

  }, [imageData]);

  return (
    <div className="app">
      {imageData.map((data, index) => (
        <div className="image-container" key={index}>
          <p>Class: {data.cls}</p>
          <p>Confidence: {data.confidence}</p>
          <p>Timestamp: {data.timestamp}</p>
          {/* <img
            className={data.confidence > 0.3 ? 'highlighted-image' : ''}
            src={`data:image/jpeg;base64, ${data.image}`}
            alt={`Image ${index}`}
          /> */}
        </div>
      ))}
    </div>
  );
};

export default App;
