const express = require('express');
const cors = require('cors');

const app = new express();
const port = 8080;

// Admin shit
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(cors());

app.get("/",(req,res)=>{
    res.render('index.html');
  });

app.post("/coords", (req,res) => {
    console.log("Request received: " + JSON.stringify(req.body));
    res.set('Content-Type', 'text/plain');
    res.send("You sent a request to backend!");
});

// Server
app.listen(port, function(){
    console.log(`Listening on URL http://localhost:${port}`);
})