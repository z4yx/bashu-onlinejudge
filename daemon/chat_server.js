var http = require('http');
var WebSocketServer = require('ws').Server;

var crypto = require('crypto');
var iv='7284565820000000';
var key=crypto.createHash('sha256').update('my)(password_xx0').digest();

function decode_user_id(str)
{
	var decoded;
	try {
		var decipher = crypto.createDecipheriv('aes-256-cbc', key, iv);
		decoded = decipher.update(str, 'base64', 'utf8');
		decoded += decipher.final('utf8');
		if(!(/^id-\w+$/.test(decoded)))
			throw 0;
		decoded = decoded.substring(3);
	}catch(e) {
		return null;
	}
	return decoded;
}
function getTime()
{
	var now = new Date();
	var h = now.getHours(), m = now.getMinutes(), s = now.getSeconds();
	return '['+(h < 10 ? '0' : '')+h+':'+(m < 10 ? '0' : '')+m+':'+(s < 10 ? '0' : '')+s+']';
}

var clients = {}, total = 0, online = 0;

var srv = http.createServer();
srv.listen(6844, function() {
	console.log('Server started.');
	var wss = new WebSocketServer({server : srv});

	wss.on('connection', function(ws) {
		var client_id = ++total;
		++online;
		console.log('connected %d', client_id);
		clients[client_id] = ws;

		ws.on('message', function(message) {
			console.log('received: %s', message);

			if(!ws.hasOwnProperty('user_id')){
				var user = decode_user_id(message);
				if(user === null)
					ws.close();
				ws.user_id = user;
				return;
			}

			var msg = getTime() + '[' + ws.user_id + '] ' + message;
			for(var i in clients) {
				clients[i].send(msg);
			}
		});
		ws.on('close', function(){
			--online;
			delete clients[client_id];
			console.log('close %d', client_id);
		});
	});
});