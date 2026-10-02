function main() {
    console.log("Starting loop");
    readcfile();
    loop();
}

function readcfile() {
    f = std.open("run.c", "r");
    console.log(f.readAsString());
    f.close();
}

function loop() {
    for (var i = 0; ; i++) {
        printc(i);
        printc(i, i * 1.25, i / 2, "a" + "b" + "c");
        delay(1);
    }
}

main();
