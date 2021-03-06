import React, {useState} from 'react';
import TextField from '@material-ui/core/TextField';
import {makeStyles } from '@material-ui/core/styles';
import { withStyles } from '@material-ui/core/styles';
import Switch from '@material-ui/core/Switch';
import Grid from '@material-ui/core/Grid';
import Typography from '@material-ui/core/Typography';
import Button from '@material-ui/core/Button';
import axios from 'axios';
import grid from "./grid.png";
import ground from "./background.jpg";
import { useLocation } from 'react-router-dom';
import './Home.css';
import LineTo from 'react-lineto';


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
    root:{
        marginLeft:10,
        display: 'inline-block',
        '& .MuiTextField-root':{
            margin: theme.spacing(1),             
        },
    },
    button:{
        margin: theme.spacing(2),   
    },
    typography1:{
        marginLeft:17,
        marginTop:20,
        
    },
    typography2:{
        marginLeft:17,
        marginTop:20,
    }
}));

function Home(){

    if(useLocation().pathname==="/coords"){
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

    const classes = useStyles();

    const [inputFields, setInputFields] = useState([
        {coordinateX: '', coordinateY: ''}
    ])

    //Allows the user to input text, needs modification to only accept numbers
    const handleChangeInput=(index, event)=>{
        const values=[...inputFields];
        values[index][event.target.name]=event.target.value;
        setInputFields(values);
        console.log(values);
    }

   

    const handleSubmit=(event)=>{
        for(var j in 8){
        document.getElementById(j).style.display="none"; 
        document.getElementById(j).style.top= "420px"; 
        document.getElementById(j).style.left="825px";
        }
        event.preventDefault();
        console.log("Message sent: " + JSON.stringify(inputFields[0]));
        axios.post('https://debonair.duckdns.org:8443/coords', inputFields[0])
            .then(response=>{
                console.log("Received message: " + JSON.stringify(response.data));
                //points is array of coordinates x and y for optimal path
                for( var i in response.data.points){
                    var x_disp= Math.floor(825+(response.data.points[i].x)*300/1000);
                    var y_disp=Math.floor(420-(response.data.points[i].y)*300/1000);
                   
                    document.getElementById("PAF").innerHTML=  "Point "+ i + ": [" + response.data.points[i].x + ";" + response.data.points[i].y + "] " +"to get to destination <br/>"; 
                    document.getElementById(i).style.left= x_disp + "px"; //x axis update
                    document.getElementById(i).style.top= y_disp + "px";  
                    document.getElementById(i).style.display="block"; 
                    <LineTo from={i} to={i+1}/>
               }   
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
    
}

    const [checked, setChecked] = useState(true);

    const Coordinate=(event)=>{
        if(checked===true){
        setChecked(!checked);
        event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'mode':'C' }));
        axios.post('https://debonair.duckdns.org:8443/mode', { 'mode':'C' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
        }
        else{
         setChecked(!checked);
        }
    }

    const [explore, setExplore] = useState(true);
    const Explore=(event)=>{
        if(explore===true){
        setExplore(!explore);
        event.preventDefault();
        console.log("Message sent: " + JSON.stringify({ 'mode':'E' }));
        axios.post('https://debonair.duckdns.org:8443/mode', { 'mode':'E' } )
            .then(response=>{
                console.log(JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })
        }
        else{
         setExplore(!explore);
        }
    }
    return(
        <div id="surface" style={{marginBottom:"-104px"}}>
            <img src={grid} alt="map" className="map"/>
            <img src={ground} alt="mars" className="mars"/>
            
            
            <h1 className="header"> Ground Discovery </h1>
            <h4 className="head"> Reach Coordinate </h4>
            <Typography component="div" className={classes.typography2}>
            <Grid component="label" container alignItems="center" spacing={1}>
             <Grid item>Off</Grid>
             <Grid item>
            <AntSwitch onChange={Coordinate} />
            </Grid>
            <Grid item>On</Grid>
            </Grid>
            </Typography>
            <form className={classes.root} onSubmit={event=> handleSubmit(event)}>
                {inputFields.map((inputField,index)=>(
                    <div key={index}>
                    <TextField
                        name="coordinateX"
                        label="Coordinate X"
                        type='number'
                        display='flex'
                        
                        value={inputField.coordinateX}
                        onChange={event=> handleChangeInput(index,event)}
                        />
                     <TextField
                        name="coordinateY"
                        label="Coordinate Y"
                        type='number'
                        display='flex'
                       
                        value={inputField.coordinateY}
                        onChange={event=> handleChangeInput(index,event)}
                        />
                    </div>
                ))}
                <Button className={classes.button}
                 variant="contained" 
                 color="primary" 
                 type="submit"
                 onClick={handleSubmit}>
                Submit
                </Button>
            </form>
            <div id="PAF" style={{position:"absolute",marginLeft: "10px"}}></div>
            
            <h4 className="h62"> Rover Explore </h4>
            <Typography component="div" className={classes.typography1}>
            <Grid component="label" container alignItems="center" spacing={1}>
             <Grid item>Off</Grid>
             <Grid item>
            <AntSwitch onChange={Explore} />
            </Grid>
            <Grid item>On</Grid>
            </Grid>
            </Typography>
        </div>
    );
}

export default Home;