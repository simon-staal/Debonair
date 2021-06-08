import React, { useState, useEffect } from 'react';
import grid from '../images/grid.png';

const MousePosition=()=>{

    const [position, setPosition]=  useState([
        {x: null, y: null}
    ])
    const onMouseMove=(event)=>{
        setPosition({
        x:event.clientX, 
        y:event.clientY
        });
    };
    
    useEffect(()=>{
        window.addEventListener("mousemove", onMouseMove);
    return()=> window.removeEventListener("mousemove",onMouseMove);},[]);
    return position;
};

export default MousePosition;