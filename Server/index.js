const express = require('express');
const cors = require('cors');

const app = new express();
const port = 8080;

// Admin shit
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

const corsOptions = {
    origin: 'http://localhost:3000',
    optionsSuccessStatus: 200
}

app.use(cors(corsOptions)); // Enables CORS for just out REACT APP (both must be running on same server)

app.get("/",(req,res)=>{
    res.render('index.html');
  });

app.post("/coords", (req,res) => {
    console.log("Request received: " + JSON.stringify(req.body));

    let receivedCoord = {
        coordinateX: req.body.coordinateX,
        coordinateY: req.body.coordinateY
    }
    // res.set('Content-Type', 'text/plain');
    res.send(receivedCoord);
});

app.post("/move", (req,res) => {
    console.log("Request received: " + JSON.stringify(req.body));
    res.send("Received direction " + req.body.direction);
});

// Incorrect route
app.use((req, res, next) => {
    res.status(404).send("404: This route doesn't exist");
});

// Server
app.listen(port, function(){
    console.log(`Listening on URL http://localhost:${port}`);
})