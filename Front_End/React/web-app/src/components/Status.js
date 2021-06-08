import React from 'react';
import grid from "./grid.png";
import ground from "./background.jpg";
import axios from 'axios';
import { useLocation } from 'react-router-dom';
import BatteryFullIcon from '@material-ui/icons/BatteryFull';
import Battery80Icon from '@material-ui/icons/Battery80';

import './Status.css';

function Status(){

    if(useLocation().pathname=="/status"){
        document.getElementById('pinkball').style.display="block";
        document.getElementById('blueball').style.display="block";
        document.getElementById('greenball').style.display="block";
        document.getElementById('orangeball').style.display="block";
        document.getElementById('start').style.display="block";
        document.getElementById('reset').style.display="block";
        document.getElementById('i1').style.display="block";
        
      }
      
    var battery;

    function getBattery(){

    axios.get('https://debonair.duckdns.org:8443/battery')
    .then(response=>{
        battery=response.data.battery;})
    .then(()=>{
        if(100>=battery>=90){
            <BatteryFullIcon></BatteryFullIcon>
        }
        if(90>=battery>=80){}
    })
    }

    return(
        <div>
        <img src={grid} alt="map" className="grid"/>
        <img src={ground} alt="ground" className="ground"/>
        <h1 className="header">Rover's Health</h1>

        </div>
    );
}
//create two for loops to render the tiles
export default Status;