import { useState } from "react"
import video from "./Videos/What is Node js_.mp4"
import { InputCamera } from "./Components/InputCamera"
import { CameraList } from "./Components/CameraList"
// import Video from 'react-native-video';


export default function App()
{
  const[it, setIt] = useState(1)
  const[streams, setStream] = useState([])
  const[selectedStream, setSelectedStream] = useState(null)

  function addCamera(url){
    setStream(currentStream =>{
        return[
            ...currentStream,
            {
            id: it,
            url: url
            }
        ]
        })
        setIt(curr => curr + 1)
  }
  function handleDelete(id){
    if(id === selectedStream)
    {
      setSelectedStream(null)
    }
    setStream(currentStream =>{
      return (currentStream.filter( stream=> stream.id !== id))
    })
  }
  function handleRadioChange(id) {
    setSelectedStream(id)
  }
  const show = streams.length!==0&&selectedStream? streams.filter(e=> e.id === selectedStream)[0].url:selectedStream
  return (
    <>
      <InputCamera onSubmit={addCamera}/>

      <h2> Stream list </h2>
      {/* <CameraList todos = {todos}/> */}
      <ul className="stream-list">
        {streams.length === 0 && "No streams to be shown"}
        {streams.map(stream =>{
          return (
            <div key={stream.id}>
              <label>
                <input className="" type="radio" checked={stream.id === selectedStream} onChange={() => handleRadioChange(stream.id)}/>
                Broker {stream.id} ({stream.url})
              </label>
              <button className="btn btn-danger" onClick={()=>handleDelete(stream.id)}>Delete Stream</button>
            </div>
          )
        })}
      </ul>

      <h2>Stream Selected:</h2>
      {!selectedStream && "No Broker selected"}
      {show}
      {selectedStream && <video src={show} width="400" height="200" controls/>}
      {/* {selectedStream && <video src={video} width="800" height="400" controls/>} */}
      
    </>
  )
}