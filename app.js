net = require('net');

var s = net.createServer( function (socket) {
    socket.on('data', function (data) {
        var command_string = data.toString();
        var command = command_string.substr(0,1).toUpperCase();//맨 앞 글자 추출
        var value = parseInt(command_string.substr(command_string.indexOf(".")+1, command_string.length))//.뒤 글자 추출
        if (isNaN(value)){//뒤로 온 명령어가 숫자가 아니면
            console.log("Please input integer value");
            socket.write("Please input integer value\n");
        }
        else if ((value<0)||(value>100)){
            console.log("Please input value between 0 and 100");
            socket.write("Please input value between 0 and 100\n");
        }
        else if (command.indexOf('7')*command.indexOf('8')*command.indexOf('F')*command.indexOf('B')*command.indexOf('L')*command.indexOf('R')){
            console.log("Please input Proper Command(F,B,L,R)");
            socket.write("Please input Proper Command(F,B,L,R)\n");
        }
        else{
            console.log("command = " + command + "\n" + "value = " + String(value));
            socket.write("command = " + command + "\n" + "value = " + String(value) + "\n");
        }
    });
});

s.addListener('connection', function (c) {
    c.write('[tcp/ip] hello Dr.one!\n');
    c.write('How to use :\n')
    c.write('Available Command : F(Forward), B(Backward), L(Left), R(Right)\n')
    c.write('Available Value : 0~100(integer)\n')
    c.write('Command.Value\n')
    c.write('ex)F.60, L.73 B.40\n')
});
s.listen(23452);

console.log('start tcp~~~');

