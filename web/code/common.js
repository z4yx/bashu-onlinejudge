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
shortcuts={
	"65": function(){
			try{
				$('ul.pager>li>a.pager-pre-link').get(0).click();
			}catch(exp){}
		} , //alt+A
	"68": function(){
			try{
				$('ul.pager>li>a.pager-next-link').get(0).click();
			}catch(exp){}
		} , //alt+D
	"66": function(){location.href=$('#nav_bbs').attr('href');} , //alt+B
	"80": function(){location.href=$('#nav_prob').attr('href');} , //alt+P
	"82": function(){location.href=$('#nav_record').attr('href');} , //alt+R
	"73": function(){$("#search_input").focus();} , //alt+I
	"76": function(){$("#login_btn").click();} , //alt+L
	"77": function(){
			var obj=$('#nav_mail');
			if(obj.length) //if logged in
				location.href=obj.attr('href');
		} , //Alt+M

};
function reg_hotkey (key, fun) {
	shortcuts[key] = fun;
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
	$('#LoginModal').on('shown',function(){
		$('#uid').focus();
	});
	$('#logoff_btn').click(function(){$.ajax({url:"logoff.php",dataType:"html",success:function(){location.reload();}});});
	var $search_input=$('#search_input');
	if($search_input.length)
		$search_input.typeahead({
			source:function(query, update){
				$.getJSON("ajax_search.php?q="+encodeURIComponent(query),function (r){
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
}).keydown(function(E){
	console.log(E);
	if(E.altKey && !E.metaKey){
		var key=E.keyCode;
		if(key>=97 && key<=122)
			key-=32;
		if(shortcuts.hasOwnProperty(key))
			(shortcuts[key])(E);
		return false;
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

	