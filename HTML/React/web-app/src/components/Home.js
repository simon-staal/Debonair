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
    const handleChangeInput=(event)=>{
        const values=[...inputFields];
        values[event.target.name]=event.target.value;
        setInputFields(values);
        console.log(values);
    }

    const handleSubmit=(event)=>{
        event.preventDefault();
        const coordinates={
            coordinateX: inputFields.coordinateX,
            coordinateY: inputFields.coordinateY
        }
        axios.post('localhost:8080', coordinates)
            .then(response=>{
                //setInputFields(response.coordinates)
                console.log(response.coordinates)
        })
    }

    return(
        <Container>
            <h1> Home Page </h1>
            <h2>Coordinates</h2>
            <form className={classes.root} onSubmit={event=> handleSubmit(event)}>
                    <div>
                    <TextField
                        name="coordinateX"
                        label="Coordinate X"
                        value={inputFields.coordinateX}
                        onChange={event=> handleChangeInput(event)}
                        />
                     <TextField
                        name="coordinateY"
                        label="Coordinate Y"
                        value={inputFields.coordinateY}
                        onChange={event=> handleChangeInput(event)}
                        />
                    </div>
                <Button className={classes.button}
                 variant="contained" 
                 color="primary" 
                 type="submit">
                Submit
                </Button>
            </form>
        </Container>
    );
}

export default Home;