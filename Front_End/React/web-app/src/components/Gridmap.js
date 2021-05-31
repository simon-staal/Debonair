import React from 'react';
import './Gridmap.css';


function Gridmap(){
    let board=[];

    for(let i=999; i>=0;i--){
        for(let j=0; j<1000; j++){
                board.push(
            <div className="box">  </div>)
        }
    }
    return(
      <div class="gridmap" >
          {board}
      </div> 
    );
}
//create two for loops to render the tiles
export default Gridmap;