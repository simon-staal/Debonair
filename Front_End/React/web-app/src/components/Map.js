import React from 'react';
import './Map.css';
import RotateLeftIcon from '@material-ui/icons/RotateLeft';
import RotateRightIcon from '@material-ui/icons/RotateRight';
import axios from 'axios';


function Map(){

    //SetTimeout : calls a function or runs some code after a period of time, specified using the second argument (in milliseconds)

    const handleClick=(event)=>{ 
        if (event.type === "mousedown") {
            event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'L' }));
        axios.post('https://debonair.duckdns.org:8443/move', { 'direction':'L' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
        } else {
            event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'S' }));
        axios.post('https://debonair.duckdns.org:8443/move', { 'direction':'S' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            });
        }
    }
    const handleClick2=(event)=>{
        if (event.type === "mousedown") {
            event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'F' }));
        axios.post('https://debonair.duckdns.org:8443/move', { 'direction':'F' })
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
        } else {
            event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'S' }));
        axios.post('https://debonair.duckdns.org:8443/move', { 'direction':'S' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            });
        }
    }
    const handleClick3=(event)=>{
        if (event.type === "mousedown") {
            event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'B' }));
        axios.post('https://debonair.duckdns.org:8443/move', { 'direction':'B' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
        } else {
            event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'S' }));
        axios.post('https://debonair.duckdns.org:8443/move', { 'direction':'S' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            });
        }
    }
    const handleClick4=(event)=>{
        if (event.type === "mousedown") {
            event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'R' }));
        axios.post('https://debonair.duckdns.org:8443/move', { 'direction':'R' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
        } else {
            event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'direction':'S' }));
        axios.post('https://debonair.duckdns.org:8443/move', { 'direction':'S' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            });
        }
    }
 
    //hold down the button 
    return(
        <nav>
            <h1> Map Page </h1> 
            <button className="leftrotate" onMouseDown={handleClick} onMouseUp={handleClick} >
           <RotateLeftIcon/>
            </button> 
            <button className="uparrow"  onMouseDown={handleClick2} onMouseUp={handleClick2}  >
           <i class="fas fa-angle-up"></i>
            </button> 
            <button className="downarrow"  onMouseDown={handleClick3} onMouseUp={handleClick3}   >
           <i class="fas fa-angle-down"></i>
            </button> 
            <button className="rightrotate" onMouseDown={handleClick4} onMouseUp={handleClick4} >
           <RotateRightIcon/>
            </button>
         </nav>
    );
}

export default Map;