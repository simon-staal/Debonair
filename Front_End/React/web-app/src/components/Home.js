import React, { useState} from 'react';
import Container from '@material-ui/core/Container';
import TextField from '@material-ui/core/TextField';
import {makeStyles} from '@material-ui/core/styles';
import Button from '@material-ui/core/Button';
import axios from 'axios';

const useStyles = makeStyles((theme)=>({
    root:{
        '& .MuiTextField-root':{
            margin: theme.spacing(1),
        },
    },
    button:{
        margin: theme.spacing(1),
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
        axios.post('http://3.8.182.14:8080/coords', inputFields[0])
            .then(response=>{
                //setInputFields(response.coordinates)
                console.log("Received message: " + JSON.stringify(response.data));
            })
            .catch(err => {
                console.log("Received error: " + err);
            })

    }

    return(
        <Container>
            <h1> Home Page </h1>
            <h2>Coordinates</h2>
            <form className={classes.root} onSubmit={event=> handleSubmit(event)}>
                {inputFields.map((inputField,index)=>(
                    <div key={index}>
                    <TextField
                        name="coordinateX"
                        label="Coordinate X"
                        value={inputField.coordinateX}
                        onChange={event=> handleChangeInput(index,event)}
                        />
                     <TextField
                        name="coordinateY"
                        label="Coordinate Y"
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
        </Container>
    );
}

export default Home;