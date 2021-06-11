const testAddon = require('./build/Release/testaddon.node');
console.log('addon',testAddon);
//console.log('hello: ',testAddon.hello());
//console.log('add: ',testAddon.add(5, 10));
let i = 0;
let total = 0;
for(i; i < 10000; i++) {
    let start = Date.now();
    console.log('genPath: ',testAddon.genPath("0,0","3500,5000","{1000,1450}{2230,3100}{2700,3600}{3000,4450}{3350,4550}"));
    let end = Date.now()
    total += (end-start);
}

console.log("Time taken (avg of 10000 runs): "+(total/i)+"ms");
module.exports = testAddon;