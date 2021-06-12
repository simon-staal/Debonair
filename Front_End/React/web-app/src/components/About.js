import React from 'react';
import { useLocation } from 'react-router-dom';
import Mars from "./mars.mp4";
import './About.css';

function About(){    
    if(useLocation().pathname==="/"){
        document.getElementById('pinkball').style.display="none";
        document.getElementById('blueball').style.display="none";
        document.getElementById('greenball').style.display="none";
        document.getElementById('orangeball').style.display="none";
        document.getElementById('blackball').style.display="none";
        document.getElementById('start').style.display="none";
        document.getElementById('reset').style.display="none";
        document.getElementById('i1').style.display="none";
        document.getElementById('msg').style.display="none";
        document.getElementById('pinkballcoord').style.display="none";
        document.getElementById('greenballcoord').style.display="none";
        document.getElementById('orangeballcoord').style.display="none";
        document.getElementById('blueballcoord').style.display="none";
        document.getElementById('blackballcoord').style.display="none";
      }

    return(
        <div className="div1">
            <h1 className="header1">Rover Debonair</h1>
            <video src={Mars} style={{width:"100%", heigh:"400px" }}type="video/mp4" autoPlay loop muted/>
            
            <div className="content">
            <h1 className="title">Our Mission</h1>
            <p className="text">
                <br/>
                Our teams goal was to design and develop <span className="font">Debonair</span>: an autonomous Rover capable of remotely discovering <br/> its environment 
                without any direct supervision. As it discovers its surroundings, <br/> it can detect
                and avoid obstacles, mapping along the way its environment <br/> and recording it on the website.
                Debonair exchanges data with the web-server, providing information such as the position <br/>of its encountered
                obstacles, its own location and its battery level for the User. 
            
                <br/>
                <br/>    
            </p>
            </div>

            <div className="content2" >
            <h1 className="title2">Rover's Feature</h1>
            <br/>
            <p className="text1">
                Over the past month, multiple features have been worked on and improved including: <br/>
                <br/>
                <p className="list">
                <span className="font">EXPLORATION MODE</span> Debonair discovers and maps its environment by locating all the obstacles <br/><br/>
                <span className="font">COORDINATE MODE</span> The User inputs a pair of coordinates which the Rover reaches using our optimised Path Finding Algorithm [PAF]. <br/><br/>
                <span className="font">REMOTE CONTROL MODE</span>  The User manually controls the movements of the Rover<br/><br/>
                <span className="font">BATTERY DISPLAY</span> Shows the remaining battery level of the Rover <br/>
                </p>
            </p>
            </div>
            
            <div className="content3">
            <h1 className="title3">About Us</h1>
            
            <p className="text2">
                <br/>
                Debonair has been developed and tested by a team of 6 Imperial College London students from May to June 2021 <br/>
                The design and development was divided into 6 subsystems:
                <br/><br/>
                <p className="list">
                [Command subsystem]: Frontend: Design of website - Yuna Valade<br/>
                [Control subsystem]: Backend: Rover's communication between the website - Simon Staal <br/>
                [Drive subsystem]: Rover's mobility and position control - Margherita Contri<br/>
                [Integration subsystem]: Hardware communication and PAF - Ruwan Silva<br/>
                [Power subsystem]: Rover's power alimentation - Yash Rajput <br/>
                [Vision subsystem]: Rover's recognition of obstacles -  Khayle Torres<br/>
                </p>
                <br/> 
            </p>
            </div>            
        
            
            
        </div>           
    );
}
//create two for loops to render the tiles
export default About;