const testAddon = require('./build/Release/testaddon.node');
console.log('addon',testAddon);
console.log('hello: ',testAddon.hello());
console.log('add: ',testAddon.add(5, 10));
console.log('test: ',testAddon.test('a','b','c'));
module.exports = testAddon;