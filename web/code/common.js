function htmlEncode(str) {
	var s = "";
	if (str.length == 0) return "";
	s = str.replace(/&/g, "&amp;");
	s = s.replace(/ /g, "&nbsp;");
	s = s.replace(/</g, "&lt;");
	s = s.replace(/>/g, "&gt;");  
	s = s.replace(/\'/g, "&#39;");
	s = s.replace(/\"/g, "&quot;");
	return s;
}
function encode_space(str) {
	var s="";
	if(str.length == 0) return "";
	s=str.replace(/\r?\n/g, "<br>");
	s=s.replace(/ /g, "&nbsp;");
	s=s.replace(/\t/g, "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	return s;
}
$(document).ready(function(){
	var $nav=$('#navbar_top'),navFixed=false,$win=$(window),$container=$('body>.container-fluid'),$notifier=$('#notifier');
	function processScroll () {
		var now = $win.scrollTop(),
			navTop = $('header').height();
		if(now>navTop && !navFixed){
			navFixed = true;
			$container.css('margin-top','62px');
			$nav.addClass('navbar-fixed-top');
		}else if(now<=navTop && navFixed){
			navFixed = false;
			$container.css('margin-top','0');
			$nav.removeClass('navbar-fixed-top');
		}
	}
	if($nav.length){
		processScroll();
		$win.on('scroll', processScroll);
	}
	$('#logoff_btn').click(function(){$.ajax({url:"logoff.php",dataType:"html",success:function(){location.reload();}});});
	var $search_input=$('#search_input');
	if($search_input.length)
		$search_input.typeahead({
			source:function(query, update){
				console.log(query);
				$.getJSON("ajax_search.php?q="+encodeURIComponent(query),function (r){
					  console.log(r);
					  update(r.arr);
					}
				);
			}
		});
	$('#form_login').submit(function(E){
		var b=false;
		if($('#uid').attr('value')==''){
			$('#uid_ctl').addClass('error');
			b=true;
		}else{
			$('#uid_ctl').removeClass('error');
		}
		if($('#pwd').attr('value')==''){
			b=true;
			$('#pwd_ctl').addClass('error');
		}else
			$('#pwd_ctl').removeClass('error');
		if(b){
			return false;
		}
	});
	$('#search_form').submit(function(){
		if($.trim($('#search_input').val()).length==0)
			return false;
		return true;
	});
	function checkMail()
	{
		$.get("ajax_checkmail.php",function(data){
			if(isNaN(data)||data=='0')
				return;
			$notifier.html('&nbsp;('+data+')');
			var $alert=$('<div class="alert alert-success center nocontent">You have unread mails.</div>').appendTo('body');
			setTimeout(function(){$alert.fadeOut(400);},1000);
		});
	}
	if($notifier.length) {
		setTimeout(checkMail,3000);
	}
});
$(function(){
if($.browser.msie&&parseInt($.browser.version,10)===6){
	$('.row div[class^="span"]:last-child').addClass("last-child");
	$('[class="span"]').addClass("margin-left-20");
	$(':button[class="btn"], :reset[class="btn"], :submit[class="btn"], input[type="button"]').addClass("button-reset");
	$(":checkbox").addClass("input-checkbox");
	$('[class^="icon-"], [class=" icon-"]').addClass("icon-sprite");
	$(".pagination li:first-child a").addClass("pagination-first-child")
}
}); 

	