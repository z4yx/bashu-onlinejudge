<?php
session_start();
if(!isset($_SESSION['administrator']))
	die('Not administrator');
if(!isset($_POST['op']))
	die('error');
$op=$_POST['op'];
require('inc/database.php');
if($op=="list_usr"){ 
	$res=mysql_query("select user_id,accesstime from users where defunct='Y'");
	if(mysql_num_rows($res)==0)
		die ('<div class="row-fluid"><div class="alert alert-info span4">No disabled users</div></div>');
?>
	<table class="table table-condensed table-striped" style="width:480px">
		<caption>Disabled users</caption>
		<thead>
			<tr>
				<th>User</th>
				<th>Enable</th>
				<th>Delete</th>
			</tr>
		</thead>
		<tbody>
			<?php
				while($row=mysql_fetch_row($res)){
					echo '<tr><td>',$row[0];
					if(is_null($row[1]))
						echo '<span style="color:red">(new)</span>';
					echo '</td><td><a href="#"><i class="icon icon-ok"></i></a></td><td><a href="#"><i class="icon icon-remove"></i></a></td></tr>';
				}
			?>
		</tbody>
	</table>
<?php
}else if($op=="list_priv"){ ?>
	<table class="table table-condensed table-striped" style="width:480px">
		<caption>Privilege List</caption>
		<thead>
			<tr>
				<th>User</th>
				<th>Privilege</th>
				<th>Delete</th>
			</tr>
		</thead>
		<tbody>
			<?php 
				$res=mysql_query("select user_id,rightstr from privilege");
				while($row=mysql_fetch_row($res)){
					echo '<tr><td>',$row[0],'</td><td>',$row[1],'</td><td><a href="#"><i class="icon icon-remove"></i></a></td></tr>';
				}
			?>
		</tbody>
	</table>
<?php
}else if($op=="list_news"){
	$res=mysql_query("select news_id,time,title from news where news_id>0 order by news_id");
	if(mysql_num_rows($res)==0)
		die ('<div class="row-fluid"><div class="alert alert-info span4">Empty list</div></div>');
?>
	<table class="table table-condensed table-striped">
		<caption>News List</caption>
		<thead>
			<tr>
				<th style="width:6%">ID</th>
				<th style="width:20%">Date</th>
				<th style="width:68%">Title</th>
				<th style="width:6%">Delete</th>
			</tr>
		</thead>
		<tbody>
			<?php
				while($row=mysql_fetch_row($res)){
					echo '<tr><td>',$row[0],'</td><td>',$row[1],'</td><td style="text-align:left">',$row[2],'</td><td><a href="#"><i class="icon icon-remove"></i></a></td></tr>';
				}
			?>
		</tbody>
	</table>
<?php
}else if($op=="add_news"){
	if(!isset($_POST['news']))
		die('');
	$title=mysql_real_escape_string(trim($_POST['news']));
	$res=mysql_query("select max(news_id) from news");
	$row=mysql_fetch_row($res);
	$id=1;
	if($row[0])
		$id=$row[0]+1;
	mysql_query("insert into news(news_id,time,title) values ($id,NOW(),'$title')");
}else if($op=="add_priv"){
	isset($_POST['user_id']) ? $uid=mysql_real_escape_string(trim($_POST['user_id'])) : die('');
	if($uid=='')
		die('');
	isset($_POST['right']) ? $right=$_POST['right'] : die('');
	if($right!='administrator'&&$right!='source_browser')
		die('Invalid privilege');
	mysql_query("insert into privilege VALUES ('$uid','$right','N')");
}else if($op=="del_usr"){
	isset($_POST['user_id']) ? $uid=mysql_real_escape_string(trim($_POST['user_id'])) : die('');
	mysql_query("delete from users where user_id='$uid'");
}else if($op=="del_priv"){
	isset($_POST['user_id']) ? $uid=mysql_real_escape_string(trim($_POST['user_id'])) : die('');
	isset($_POST['right']) ? $right=mysql_real_escape_string($_POST['right']) : die('');
	mysql_query("delete from privilege where user_id='$uid' and rightstr='$right'");
}else if($op=="del_news"){
	isset($_POST['news_id']) ? $news_id=intval($_POST['news_id']) : die('');
	mysql_query("delete from news where $news_id>0 and news_id=$news_id");
}else if($op=="en_usr"){
	isset($_POST['user_id']) ? $uid=mysql_real_escape_string(trim($_POST['user_id'])) : die('');
	mysql_query("update users set defunct='N' where user_id='$uid'");
}else if($op=="disable_usr"){
	isset($_POST['user_id']) ? $uid=mysql_real_escape_string(trim($_POST['user_id'])) : die('');
	mysql_query("update users set defunct='Y' where user_id='$uid'");
}else if($op=='update_index'){
	$index_text=isset($_POST['text']) ? mysql_real_escape_string(str_replace("\n", "<br>", $_POST['text'])) : '';
	if(mysql_query("insert into news (news_id,content) VALUES (0,'$index_text') ON DUPLICATE KEY UPDATE content='$index_text'"))
		echo "success";
	else
		echo "fail";
}
?>