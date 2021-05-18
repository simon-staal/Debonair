Notes
=====
**18/05/21**
Starting to work on building the backend server in node.js.
To run the server, simply run `node index.js` (make sure node is up to date).
To then test the server functionality, in another terminal you can use `curl localhost:8080` (in this particular case we are using port 8080)

Going to try to run server in AWS instance and see if I can connect from local PC.

*Update* THIS WORKS!!!!!!
Downloaded repo onto AWS instance, used screen to run `node index.js` in a seperate whatever the fuck, did `curl <server_ip>:8080` on local PC and received a response
