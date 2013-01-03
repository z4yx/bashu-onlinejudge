<?php
require('inc/checklogin.php');

if(isset($_GET['start_id']))
	$page=intval($_GET['start_id']);
else
	$page=0;
?>
<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<title>Mail List</title>
		<meta name="viewport" content="width=device-width, initial-scale=1.0">

		<link href="../assets/css/bootstrap.css" rel="stylesheet">
		<link href="../assets/css/bootstrap-responsive.css" rel="stylesheet">
	    <link href="../assets/css/docs.css" rel="stylesheet">
		<!--[if IE 6]>
		<link href="ie6.min.css" rel="stylesheet">
		<![endif]-->
		<!--[if lt IE 9]>
			<script src="../assets/js/html5.js"></script>
		<![endif]-->
	</head>

	<body>
		<?php require('page_header.php'); ?>       
		<div class="container-fluid">
<?php 
if(!isset($_SESSION['user'])){
			echo '<div style="text-align: center">Not logged in.</div>';
}else{?>
			<div class="row-fluid"><div class="span2 offset2">
				<span id="sendnew" style="margin:5px" class="btn btn-small"><i class="icon-inbox"></i> Send Mail</span>
			</div></div>
			<div class="row-fluid" style="font-size:14px">
				<div class="span8 offset2" id="maillist">
						<ul>
<?php
	require('inc/database.php');
	$user_id=$_SESSION['user'];
	$result=mysql_query("select mail_id,title,from_user,new_mail,in_date from mail where to_user='$user_id' and UPPER(defunct)='N' order by mail_id desc limit $page,20");
	while($row=mysql_fetch_row($result)){
		echo      '<li class="mail-item" id="mail',$row[0],'">';
?>
								<div class="mail-container">
									<div class="mail-title">
										<?php 
										if($row[3])echo '<span class="label label-warning" style="padding:1px"><i class="icon-chevron-right icon-white"></i></span>';
										echo '<a href="#title">',htmlspecialchars($row[1]),'</a>';
										?>
									</div>
									<div class="mail-info"><?php echo $row[2],' at ',substr($row[4],0,10);?></div>
									<div style="clear:both"></div>
									<div class="mail-content">
										<div class="mail-op">
											Received at <?php echo substr($row[4],-8)?>
											<a href="#del" class="btn btn-mini btn-danger">
												<i class="icon-trash icon-white"></i>
												Delete
											</a>
											<a href="#rep" class="btn btn-mini btn-primary">
												<i class="icon-share-alt icon-white"></i>
												Reply
											</a>
										</div>
										<pre></pre>
									</div>
								</div>
							</li>
<?php
	}
?>
						</ul>
				</div>
			</div>  
			<div class="row-fluid">
				<ul class="pager">
					<li>
						<a href="#" id="btn-pre">&larr; Previous</a>
					</li>
					<li>
						<a href="#" id="btn-next">Next &rarr;</a>
					</li>
				</ul>
			</div>  

			<div class="modal hide" id="MailModal">
				<div class="modal-header">
					<a class="close" data-dismiss="modal">×</a>
					<h4>Send Mail</h4>
				</div>
				<div class="modal-body">
					<form class="form-horizontal" id="send_form">
						<fieldset>
							<div class="control-group">
								<label class="control-label" for="to_input">To User</label>
								<div class="controls">
									<input type="text" class="input-medium" id="to_input" name="touser">
								</div>
							</div>
							<div class="control-group">
								<label class="control-label" for="title_input">Title</label>
								<div class="controls">
									<input type="text" style="width: 445px;" id="title_input" name="title">
								</div>
							</div>
							<div class="control-group">
								<label class="control-label" for="detail_input">Detail</label>
								<div class="controls">
									<textarea style="width: 445px;" id="detail_input" rows="10" name="detail"></textarea>
								</div>
							</div>
						</fieldset>
					</form>
					<div class="alert alert-error hide" id="send_result"></div>
				</div>
				<div class="modal-footer">
					<a href="#" class="btn btn-primary" id="send_btn">Send</a>
					<a href="#" class="btn" data-dismiss="modal">Close</a>
				</div>
			</div>
<?php
}?>
			<hr>
			<footer class="muted" style="text-align: center;font-size:12px;">
				<p>&copy; 2012 Bashu Middle School</p>
			</footer>

		</div><!--/.container-->
		<script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
		<script src="common.js"></script>

		<script type="text/javascript"> 
			$(document).ready(function(){
				var cur=<?php echo $page?>;
				$('#btn-next').click(function(){
					location.href='mail.php?start_id='+(cur+20);
					return false;
				});
				$('#btn-pre').click(function(){
					if(cur-20>=0)
						location.href='mail.php?start_id='+(cur-20);
					return false;
				});
				$('#maillist').click(function(E){
					var $a;
					if($(E.target).is('i'))
						$a=$(E.target).parent();
					else if(typeof(E.target.href)!='undefined')
						$a=$(E.target);
					else
						return;
					var j=$a.attr('href'),k,content,mailid;
					switch(j.substr(j.lastIndexOf('#')+1)){
						case 'title':
							k=$a.parent().parent(); 
							mailid=k.parent().get(0).id.substr(4);
							content=k.children('.mail-content');
							if(content.is(":hidden")){
								$.get('ajax_showmail.php?mail_id='+mailid,function(data){
									if(typeof(window.fix_ie_pre)!='undefined')
										data=encode_space(data);
									content.children('pre').html(data);
								});
								content.show();
								$a.prev('span').remove();
							}else{
								content.hide();
							}
							break;
						case 'del':
							k=$a.parents('li');
							$.ajax('ajax_deletemail.php?mail_id='+k.attr('id').substr(4));
							k.remove();
							break;
						case 'rep':
							k=$a.parent().parent().prev().prev();
							content=k.html();
							$('#to_input').val(content.substr(0,content.indexOf(' ')));
							k=k.prev().children('a');
							$('#title_input').val('Re:'+k.html());
							$('#send_result').hide();
							$('#MailModal').modal('show');
							break;
					}
					return false;
				});
				$('#send_btn').click(function(){
					$.ajax({
						type:"POST",
						url:"ajax_sendmail.php",
						data:$('#send_form').serialize(),
						success:function(msg){
							if(msg.indexOf('__OK__')!=-1){
								location.reload();
								return;
							}
							$('#send_result').html(msg).show();
						}
					});
				});
				$('#sendnew').click(function(){
					$('#send_result').hide();
					$('#MailModal').modal('show');
				});
				$('#ret_url').val("mail.php");
			}); 
		</script>
	</body>
</html>
