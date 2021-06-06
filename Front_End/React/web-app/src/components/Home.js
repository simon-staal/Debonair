import React, { useState} from 'react';
import TextField from '@material-ui/core/TextField';
import {makeStyles} from '@material-ui/core/styles';
import Button from '@material-ui/core/Button';
import axios from 'axios';
import grid from "./grid.png";
import ground from "./background.jpg";
import './Home.css'


const useStyles = makeStyles((theme)=>({
    root:{
        display: 'inline-block',
        '& .MuiTextField-root':{
            margin: theme.spacing(1),             
        },
    },
    button:{
        margin: theme.spacing(2),
        
    }
}))

function Home(){
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
        event.preventDefault();
        console.log("Message sent: " + JSON.stringify(inputFields[0]));
        axios.post('https://debonair.duckdns.org:8443/coords', inputFields[0])
            .then(response=>{
                //setInputFields(response.coordinates)
                console.log("Received message: " + JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })

    }

    return(
        <div>
            <h1 className="header"> Coordinate Mode </h1>
            
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
            <img src={grid} alt="map" className="map"/>
            <img src={ground} alt="mars" className="mars"/>
            
        </div>
    );
}

export default Home;