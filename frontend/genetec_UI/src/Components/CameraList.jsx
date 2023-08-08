export function CameraList({streams}) {
  return (
    <ul className="stream-list">
      {streams.length === 0 && "No streams to be shown"}
      {streams.map((stream) => {
        return (
          <div key={stream.id}>
            <label>
              <input
                className=""
                type="radio"
                checked={stream.id === selectedStream}
                // onChange={() => handleRadioChange(stream.id)}
              />
              Camera {stream.id} ({stream.url})
            </label>
            <button
              className="btn btn-danger"
            //   onClick={() => handleDelete(stream.id)}
            >
              Delete Stream
            </button>
          </div>
        );
      })}
    </ul>
  );
}
