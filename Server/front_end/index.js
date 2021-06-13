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
                if(i === 500) process.exit(0);
				start = Date.now();
				axios.get('https://debonair.duckdns.org:8443/')
                .then(response=>{
                    end = Date.now();
                    console.log("Received response")
                    total += (end-start);
                    vals.push(end-start);
                    i++;
                    if(i === 500) {
                        console.log("Average time for "+i+" runs = "+(total/i)+"ms");
                        console.log(vals.toString());
                        fs.writeFile('./results.csv',vals.toString(), err => {
                            if(err) console.log(err)
                        });
                        //process.exit(0);
                    }
                })
                .catch(err => {
                    console.log("Received error: " + err);
                    return;
                })
    			},100);
	}, 1000);
}

sendTest();
