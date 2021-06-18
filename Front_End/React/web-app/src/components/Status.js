import React from 'react';
import grid from "./grid.png";
import ground from "./background.jpg";
import axios from 'axios';
import Button from '@material-ui/core/Button';
import { useLocation } from 'react-router-dom';
import BatteryFullIcon from '@material-ui/icons/BatteryFull';
import Battery90Icon from '@material-ui/icons/Battery90';
import Battery80Icon from '@material-ui/icons/Battery80';
import Battery60Icon from '@material-ui/icons/Battery60';
import Battery50Icon from '@material-ui/icons/Battery50';
import Battery30Icon from '@material-ui/icons/Battery30';
import Battery20Icon from '@material-ui/icons/Battery20';
import BatteryAlertIcon from '@material-ui/icons/BatteryAlert';
import {makeStyles} from '@material-ui/core/styles';
import './Status.css';

let stop = 0;

const useStyles = makeStyles((theme)=>({
   button:{
       margin: theme.spacing(2),   
   },
   
}));

function Status(){
   var battery;
   var health; 
    
   const classes = useStyles();

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
        document.getElementById('info').style.display="block";
        document.getElementById('pinkballcoord').style.display="block";
        document.getElementById('greenballcoord').style.display="block";
        document.getElementById('orangeballcoord').style.display="block";
        document.getElementById('blueballcoord').style.display="block";
        document.getElementById('blackballcoord').style.display="block";
     }

const getBattery=()=>{

    axios.get('https://debonair.duckdns.org:8443/battery')
    .then(response=>{
      battery=response.data.battery;
      health=response.data.SOH;
      })
   .catch(err => {
      console.log(err);
      stop = 1;
   })
    .then(()=>{
      document.getElementById("level").innerHTML=battery+ "%"; 
      if(100>=health && health>=50){
         document.getElementById("SOH").innerHTML="Debonair is healthy! State of health: " + health +"%"; 
      }
      else if(50>health && health>20){
         document.getElementById("SOH").innerHTML="Debonair's state of health is moderate: " + health +"%";
      }
      else if(health>=20){
         document.getElementById("SOH").innerHTML="Debonair needs servicing. State of health: " + health +"%";
      }
        if(battery===100||(100>battery && battery>90)){
            document.getElementById("Full").style.display = "block";  
            document.getElementById("90").style.display = "none";
            document.getElementById("80").style.display = "none";
            document.getElementById("60").style.display = "none";
            document.getElementById("50").style.display = "none";
            document.getElementById("30").style.display = "none";
            document.getElementById("20").style.display = "none";
            document.getElementById("Alert").style.display = "none";          
        }
        if(battery===90||(90>battery && battery>80)){
         document.getElementById("Full").style.display = "none"; 
            document.getElementById("90").style.display = "block";
            document.getElementById("80").style.display = "none";
            document.getElementById("60").style.display = "none";
            document.getElementById("50").style.display = "none";
            document.getElementById("30").style.display = "none";
            document.getElementById("20").style.display = "none";
            document.getElementById("Alert").style.display = "none";   
            
         }
         if(battery===80||(80>battery && battery>60)){
            document.getElementById("Full").style.display = "none";
            document.getElementById("90").style.display = "none";
            document.getElementById("80").style.display = "block";
            document.getElementById("60").style.display = "none";
            document.getElementById("50").style.display = "none";
            document.getElementById("30").style.display = "none";
            document.getElementById("20").style.display = "none";
            document.getElementById("Alert").style.display = "none";    
         }
         if(battery===60||(60>battery && battery>50)){
            document.getElementById("Full").style.display = "none";
            document.getElementById("90").style.display = "none";
            document.getElementById("80").style.display = "none";
            document.getElementById("60").style.display = "block";
            document.getElementById("50").style.display = "none";
            document.getElementById("30").style.display = "none";
            document.getElementById("20").style.display = "none";
            document.getElementById("Alert").style.display = "none";    
         }
         if(battery===50||(50>battery && battery>30)){
            document.getElementById("Full").style.display = "none";
            document.getElementById("90").style.display = "none";
            document.getElementById("80").style.display = "none";
            document.getElementById("60").style.display = "none";
            document.getElementById("50").style.display = "block";
            document.getElementById("30").style.display = "none";
            document.getElementById("20").style.display = "none";
            document.getElementById("Alert").style.display = "none";    
         }
         if(battery===30||(30>battery && battery>20)){
            document.getElementById("Full").style.display = "none";
            document.getElementById("90").style.display = "none";
            document.getElementById("80").style.display = "none";
            document.getElementById("60").style.display = "none";
            document.getElementById("50").style.display = "none";
            document.getElementById("30").style.display = "block";
            document.getElementById("20").style.display = "none";
            document.getElementById("Alert").style.display = "none";    
         }
         if(battery===20||(20>battery && battery>10)){
            document.getElementById("Full").style.display = "none";
            document.getElementById("90").style.display = "none";
            document.getElementById("80").style.display = "none";
            document.getElementById("60").style.display = "none";
            document.getElementById("50").style.display = "none";
            document.getElementById("30").style.display = "none";
            document.getElementById("20").style.display = "block";
            document.getElementById("Alert").style.display = "none";    
         }
         if(10>=battery){
            document.getElementById("Full").style.display = "none";
            document.getElementById("90").style.display = "none";
            document.getElementById("80").style.display = "none";
            document.getElementById("60").style.display = "none";
            document.getElementById("50").style.display = "none";
            document.getElementById("30").style.display = "none";
            document.getElementById("20").style.display = "none";
            document.getElementById("Alert").style.display = "block";    
         }
        
      })
      .catch(err => {
         console.log(err);
         stop = 1;
      })
   }   
   
   const run=()=>{
      enable();
      batterylevel();
   }

   const enable=()=>{
      stop = 0;
   }
   
   const batterylevel=()=>{
         if(stop){
            if (timer) {
               clearTimeout(timer);
               timer = 0;
            }
         getBattery();
         timer = setTimeout(batterylevel,3000)
        }
    
    
    return(
        <div id="batteryshow" style={{marginBottom:"229px"}}>
        <img src={grid} alt="map" className="grid"/>
        <img src={ground} alt="ground" className="ground"/>
        <h1 className="header">Rover's Health</h1>
        <Button className={classes.button}
                 variant="contained" 
                 color="primary" 
                 type="button"
                 onClick={run}>
                Battery Level
                </Button>
         <div id="level" style={{ position:"absolute", marginLeft:"200px", marginTop:"75px", fontSize:"50px", fontWeight:"bold"}}> </div>
         <div id="SOH" style={{position:"absolute", marginTop:"250px", marginLeft:"20px", fontSize:"20px"}}></div>
        
        <div style={{position:"absolute", marginTop:"-25px"}}>
        <BatteryFullIcon id="Full" style={{ width:"200px", height:"300px", marginLeft:"20px", display:"none", color:"#30b455"}}> </BatteryFullIcon>
        <Battery90Icon id="90" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none",color:"#30b455"}} ></Battery90Icon>
        <Battery80Icon id="80" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none",color:"#30b455"}} ></Battery80Icon>
        <Battery60Icon id="60" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none",color:"#30b455"}} ></Battery60Icon>
        <Battery50Icon id="50" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none", color:"#EFAF13"}} ></Battery50Icon>
        <Battery30Icon id="30" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none", color:"#EFAF13"}}></Battery30Icon>
        <Battery20Icon id="20" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none", color:"#EFAF13"}} ></Battery20Icon>
        <BatteryAlertIcon id="Alert" style={{width:"200px", height:"300px",marginLeft:"20px", display:"none", color:"#e81309"}} ></BatteryAlertIcon>
        </div>
        </div>
    );
}
//create two for loops to render the tiles
export default Status;