import React, { useState} from 'react';
import './Map.css';
import RotateLeftIcon from '@material-ui/icons/RotateLeft';
import RotateRightIcon from '@material-ui/icons/RotateRight';
import axios from 'axios';
import grid from "./grid.png";
import ground from "./background.jpg";
import Switch from '@material-ui/core/Switch';
import Grid from '@material-ui/core/Grid';
import Typography from '@material-ui/core/Typography';
import {makeStyles } from '@material-ui/core/styles';
import { withStyles } from '@material-ui/core/styles';
import { useLocation } from 'react-router-dom';
import MousePosition from './position.js';


const AntSwitch = withStyles((theme) => ({
    root: {
      width: 28,
      height: 16,
      padding: 0,
      display: 'flex',
    },
    switchBase: {
      padding: 2,
      color: theme.palette.grey[500],
      '&$checked': {
        transform: 'translateX(12px)',
        color: theme.palette.common.white,
        '& + $track': {
          opacity: 1,
          backgroundColor: theme.palette.primary.main,
          borderColor: theme.palette.primary.main,
        },
      },
    },
    thumb: {
      width: 12,
      height: 12,
      boxShadow: 'none',
    },
    track: {
      border: `1px solid ${theme.palette.grey[500]}`,
      borderRadius: 16 / 2,
      opacity: 1,
      backgroundColor: theme.palette.common.white,
    },
    checked:{},
  }))(Switch);

  const useStyles = makeStyles((theme)=>({
    typography1:{
        marginLeft:10,
        marginTop:10,
    },
}));

function Map(){

    if(useLocation().pathname==="/controller"){
        document.getElementById('pinkball').style.display="block";
        document.getElementById('blueball').style.display="block";
        document.getElementById('greenball').style.display="block";
        document.getElementById('orangeball').style.display="block";
        document.getElementById('start').style.display="block";
        document.getElementById('reset').style.display="block";
        document.getElementById('i1').style.display="block";
        document.getElementById('msg').style.display="block";
        document.getElementById('pinkballcoord').style.display="block";
        document.getElementById('greenballcoord').style.display="block";
        document.getElementById('orangeballcoord').style.display="block";
        document.getElementById('blueballcoord').style.display="block";
        
      }
    const position=MousePosition();
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
    const classes = useStyles();
    const [control, setControl] = useState(true);
    const RemoteControl=(event)=>{
        if(control===true){
        setControl(!control);
        event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'Mode':'M' }));
        axios.post('https://debonair.duckdns.org:8443/mode', { 'Mode':'M' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
        }
        else{
         setControl(!control);
        }
    }
    //hold down the button 

   
    return(
        <nav className="div">
            <img src={grid} alt="map" className="grid"/>
            <img src={ground} alt="ground" className="ground"/>
            <h1 className="header"> Remote Control </h1> 
            <Typography component="div" className={classes.typography1}>
            <Grid component="label" container alignItems="center" spacing={1}>
             <Grid item>Off</Grid>
             <Grid item>
            <AntSwitch onChange={RemoteControl} />
            </Grid>
            <Grid item>On</Grid>
            </Grid>
            </Typography>
            <div className="div">
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

            <h6 style={{marginLeft: "10px", marginTop: "150px", position:"absolute"}}>
                Click on position
            </h6>
            <div style={{marginLeft: "10px", marginTop: "170px", position: "absolute"}} > 
            {Math.round((position.x-850)/300*1000)}:{-Math.round((position.y-445)/300*1000)}</div>
            </div>
         </nav>
    );
}

export default Map;