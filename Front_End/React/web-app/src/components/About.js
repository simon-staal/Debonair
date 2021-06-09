import React from 'react';
import { useLocation } from 'react-router-dom';
import Mars from "./mars.mp4";
import './About.css';

function About(){    
    if(useLocation().pathname=="/"){
        document.getElementById('pinkball').style.display="none";
        document.getElementById('blueball').style.display="none";
        document.getElementById('greenball').style.display="none";
        document.getElementById('orangeball').style.display="none";
        document.getElementById('start').style.display="none";
        document.getElementById('reset').style.display="none";
        document.getElementById('i1').style.display="none";
      }

    return(
        <div className="div1">
            <h1 className="header1">Rover Debonair</h1>
            <video src={Mars} style={{width:"1800px", heigh:"400px" }}type="video/mp4" autoPlay loop muted/>
            <div className="content">
            <h1>Welcome</h1>
            <p>
                This project has been realised by :
            </p>
            </div>
        
        </div>
    );
}
//create two for loops to render the tiles
export default About;