const http = require('http');
const today = require('./today'); // Example of including another local node.js file

const requestListener = function (req, res) {
  res.writeHead(200);
  const date = today.getDate().getHours();
  if(date < 12){
      res.end('Good morning World!');
  }
  else if(date < 18){
      res.end('Good afternoon World!');
  }
  else{
      res.end('Good evening World!');
  }
  //res.end(`Hello, World! The date today is ${today.getDate()}`);
}

const port = 8080;
const server = http.createServer(requestListener);
console.log('server listening on port: ' + port);
server.listen(port);
