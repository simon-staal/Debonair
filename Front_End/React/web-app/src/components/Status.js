import React from 'react';
import grid from "./grid.png";
import ground from "./background.jpg";
import './Status.css';

function Status(){
    return(
        <div>
        <h1 className="header">Rover's Health</h1>
        <img src={grid} alt="map" className="grid"/>
        <img src={ground} alt="ground" className="ground"/>
        </div>
    );
}
//create two for loops to render the tiles
export default Status;