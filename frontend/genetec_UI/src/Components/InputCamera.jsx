import { useState } from "react"

export function InputCamera({onSubmit}){
    const[url, setUrl] = useState("")


    function handleSubmit(e)
    {
        e.preventDefault()
        if(url === "") return
        onSubmit(url)
        setUrl("")
    }


    return(
        <>
            <form className="new-item-form" onSubmit={handleSubmit}>
                <label className="form-row" htmlFor="item">Enter the Kafka broker</label>
                <input type="text" id="item" value={url} onChange={e => setUrl(e.target.value)}></input>
                <button className="btn">Add</button>
            </form>
        </>
    )
}