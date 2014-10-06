var socket;
function ws_connect(){
	socket = new WebSocket(ws_url);

	socket.onopen = function(){
		$('#info_socket').html('Connected');
		$('#chat_content').removeAttr('disabled').css('background-color','#fff');;
		$('#ipt_message').removeAttr('disabled');
		$('#btn_send').removeAttr('disabled');
		$('#btn_switch').removeAttr('disabled').html('Leave');

		socket.send(userid);
	};
	socket.onmessage = function(msgobj){
		var ele = $('#chat_content').append(htmlEncode(msgobj.data.replace(/\n/g, '')) + "\n").get(0);
		ele.scrollTop = ele.scrollHeight;
	};
	socket.onclose = function(){
		$('#info_socket').html('Disconnected');
		$('#chat_content').prop('disabled','disabled').css('background-color','#eee');
		$('#ipt_message').prop('disabled','disabled');
		$('#btn_send').prop('disabled','disabled');
		$('#btn_switch').removeAttr('disabled').html('Enter');
	};
}
$(document).ready(function(){
	$('#btn_switch').click(function(){
		if(typeof(ws_url)==='undefined')
			return false;
		if(!("WebSocket" in window)){
			alert('Sorry, you need a browser that supports HTML5 WebSockets.');
			return false;
		}
		if(typeof(userid)==='undefined'){
			alert("You have to log in before you can enter chat room.");
			return false;
		}
		var cmd = $(this).prop('disabled','disabled').html();
		if(cmd == 'Enter')
			ws_connect();
		else
			socket.close();
	});
	$('#btn_send').click(function(){
		var ele=$('#ipt_message');
		if(! $.trim(ele.val()))
			return false;
		socket.send(ele.val());
		ele.val('');
	});
	$('#ipt_message').keypress(function(E){
		if(E.keyCode == '13'){
			$('#btn_send').click();
			return false;
		}
	});
});