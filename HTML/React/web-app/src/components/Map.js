import React from 'react';
import './Map.css';
import RotateLeftIcon from '@material-ui/icons/RotateLeft';
import RotateRightIcon from '@material-ui/icons/RotateRight';
import axios from 'axios';


function Map(){

    
    return(
        <nav>
            <h1> Map Page </h1> 
            <button className="leftarrow" >
           <RotateLeftIcon/>
            </button> 
            <button className="uparrow" >
           <i class="fas fa-angle-up"></i>
            </button> 
            <button className="downarrow" >
           <i class="fas fa-angle-down"></i>
            </button> 
            <button className="rightarrow" >
           <i class="fas fa-angle-right"></i>
            </button>
        </nav>
    );
}

export default Map;