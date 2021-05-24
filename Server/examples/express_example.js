// To use express, declare Express as a dependency in
// package.json and run npm install

const express = require('express') 
const app = new express() // Creates instance of app object from Express web application framework
const port = 8080

// Route handler
app.get('resource_path', (req, res)=>{
    // Do stuff
});

// Start HTTP server on specified port
var server = app.listen(port, function(){
    console.log(`Listening on URL http://localhost:${port}`);
})
