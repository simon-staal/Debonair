const express = require('express');
const app = new express();

app.get("/", (req,res)=>{
    return res.send("Hello World!");
});

app.use("/user/:id", (req, res, next) => {
    if(req.query.password !== "password") {
        return res.status(402).send("This user cannot login");
    }
    console.log('Time:', Date.now());
    next();
});

app.get("/user/:id", (req, res) => {
    return res.send(`Hello ${req.params.id}, login successful`);
});

let server = app.listen(8080, ()=>{
    console.log('listening at http://localhost:'+server.address().port)
})