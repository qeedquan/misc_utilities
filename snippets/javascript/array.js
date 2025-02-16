function main() {
	testSplice();
}

function testSplice() {
	var l = [1, 2, 3, 4];
	l.splice(1, 1);
	l.splice(0, 1);
	console.log(l);
}

main();
