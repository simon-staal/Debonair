import React from 'react';
import grid from "./grid.png";
import ground from "./background.jpg";
import axios from 'axios';
import { useLocation } from 'react-router-dom';
import BatteryFullIcon from '@material-ui/icons/BatteryFull';
import Battery90Icon from '@material-ui/icons/Battery90';
import Battery80Icon from '@material-ui/icons/Battery80';
import Battery60Icon from '@material-ui/icons/Battery60';
import Battery50Icon from '@material-ui/icons/Battery50';
import Battery30Icon from '@material-ui/icons/Battery30';
import Battery20Icon from '@material-ui/icons/Battery20';
import BatteryAlertIcon from '@material-ui/icons/BatteryAlert';


import './Status.css';

function Status(){
    
    if(useLocation().pathname==="/status"){
        document.getElementById('pinkball').style.display="block";
        document.getElementById('blueball').style.display="block";
        document.getElementById('greenball').style.display="block";
        document.getElementById('orangeball').style.display="block";
        document.getElementById('blackball').style.display="block";
        document.getElementById('start').style.display="block";
        document.getElementById('reset').style.display="block";
        document.getElementById('i1').style.display="block";
        document.getElementById('msg').style.display="block";
        document.getElementById('pinkballcoord').style.display="block";
        document.getElementById('greenballcoord').style.display="block";
        document.getElementById('orangeballcoord').style.display="block";
        document.getElementById('blueballcoord').style.display="block";
        document.getElementById('blackballcoord').style.display="block";
     }
      
    var battery;
    
    function reset(){
        document.getElementById("Full").style.display = "none";
        document.getElementById("90").style.display = "none";
        document.getElementById("80").style.display = "none";
        document.getElementById("60").style.display = "none";
        document.getElementById("50").style.display = "none";
        document.getElementById("30").style.display = "none";
        document.getElementById("20").style.display = "none";
        document.getElementById("Alert").style.display = "none";
    }

    function getBattery(){

    axios.get('https://debonair.duckdns.org:8443/battery')
    .then(response=>{
      battery=response.data.battery;})
    .then(()=>{
    
        if(battery===100||(100>battery && battery>90)){
            document.getElementById("Full").style.display = "block";            
        }
        if(battery===90||(90>battery && battery>80)){
            document.getElementById("90").style.display = "block";
         }
         if(battery===80||(80>battery && battery>60)){
            document.getElementById("80").style.display = "block";
         }
         if(battery===60||(60>battery && battery>50)){
            document.getElementById("60").style.display = "block";
         }
         if(battery===50||(50>battery && battery>30)){
            document.getElementById("50").style.display = "block";
         }
         if(battery===30||(30>battery && battery>20)){
            document.getElementById("30").style.display = "block";
         }
         if(battery===20||(20>battery && battery>10)){
            document.getElementById("20").style.display = "block";
         }
         if(10>=battery){
            document.getElementById("Alert").style.display = "block";
         }
 
    })
    }
    var my_time;
    function timer(){
        reset();
        getBattery();
        my_time=setTimeout('timer()',100);
        }
    
    
    return(
        <div id="batteryshow" >
        <img src={grid} alt="map" className="grid"/>
        <img src={ground} alt="ground" className="ground"/>
        <h1 className="header">Rover's Health</h1>
        <button className="button" onClick={timer}>Battery Level</button>
        
        <BatteryFullIcon id="Full" style={{width:"200px", height:"300px", marginLeft:"20px", display:"none"}}> </BatteryFullIcon>
        <Battery90Icon id="90" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none"}} ></Battery90Icon>
        <Battery80Icon id="80" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none"}} ></Battery80Icon>
        <Battery60Icon id="60" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none"}} ></Battery60Icon>
        <Battery50Icon id="50" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none"}} ></Battery50Icon>
        <Battery30Icon id="30" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none"}}></Battery30Icon>
        <Battery20Icon id="20" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none"}} ></Battery20Icon>
        <BatteryAlertIcon id="Alert" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none"}} ></BatteryAlertIcon>
        
        </div>
    );
}
//create two for loops to render the tiles
export default Status;