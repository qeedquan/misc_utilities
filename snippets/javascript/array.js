const util = require('util');

function main() {
	testSplice();
	testPrintArray();
}

function testSplice() {
	var l = [1, 2, 3, 4];
	l.splice(1, 1);
	l.splice(0, 1);
	console.log(l);
}

// print all of the array in a copy-pastable fashion
function testPrintArray() {
	var x = [];
	for (var i = 0; i < 100; i++)
		x.push(i);
	console.log(util.inspect(x, { maxArrayLength: null }));
}

main();
