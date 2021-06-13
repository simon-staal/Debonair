const axios = require('axios');
const fs = require('fs')

let start;
let end;
let total = 0;
let i = 0;
let vals = [];
function sendTest() {
    setTimeout(() => {
			setInterval(() => {
				start = Date.now();
				axios.get('https://debonair.duckdns.org:8443/', "Hello from frontend")
                .then(response=>{
                    end = Date.now();
                    total += (end-start);
                    vals.push(end-start);
                    i++;
                    if(i === 50) {
                        console.log("Average time for "+i+" runs = "+(total/i));
                        console.log(vals.toString());
                        fs.writeFile('./results.csv',vals.toString(), err => {
                            if(err) console.log(err)
                        });
                    }
                })
                .catch(err => {
                    console.log("Received error: " + err);
                    return;
                })
    			},500);
	}, 1000);
}

sendTest();
