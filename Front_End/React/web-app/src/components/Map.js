import React, {useState} from 'react';
import './Map.css';
import RotateLeftIcon from '@material-ui/icons/RotateLeft';
import RotateRightIcon from '@material-ui/icons/RotateRight';
import axios from 'axios';


function Map(){

    const [count,setCount]= useState(0);
    const handleClick=(event)=>{
        event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'L' }));
        axios.post('http://3.8.182.14:8080/move', { 'direction':'L' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
    }
    const handleClick2=(event)=>{
        event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'F' }));
        axios.post('http://3.8.182.14:8080/move', { 'direction':'F' })
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
    }
    const handleClick3=(event)=>{
        event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'B' }));
        axios.post('http://3.8.182.14:8080/move', { 'direction':'B' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
    }
    const handleClick4=(event)=>{
        event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'R' }));
        axios.post('http://3.8.182.14:8080/move', { 'direction':'R' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
    }
    //hold down the button 
    return(
        <nav>
            <h1> Map Page </h1> 
            <button className="leftrotate" onClick={handleClick} >
           <RotateLeftIcon/>
            </button> 
            <button className="uparrow" onClick={handleClick2} >
           <i class="fas fa-angle-up"></i>
            </button> 
            <button className="downarrow" onClick={handleClick3}>
           <i class="fas fa-angle-down"></i>
            </button> 
            <button className="rightrotate" onClick={handleClick4}>
           <RotateRightIcon/>
            </button>
        </nav>
    );
}

export default Map;