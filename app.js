net = require('net');

var s = net.createServer( function (socket) {
    socket.on('data', function (data) {
        console.log(data.toString());
    });
});
s.addListener('connection', function (c) {
    c.write('[tcp/ip] hello world!\n');
});
s.listen(23452);

console.log('start tcp~~~');

