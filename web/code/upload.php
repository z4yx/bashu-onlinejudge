<?php 
session_start();
if(!isset($_SESSION['administrator']))
	die('You are not administrator');

if(isset($_FILES['file'])){
	try{
		if(!isset($_FILES['file']['name']) || !preg_match('/\.(jpg|jpeg|png|gif|bmp|tif|tiff|ico|wmf)$/i',$_FILES['file']['name']))
			throw new Exception('Invalid Image format');
		if($_FILES["file"]["error"] > 0)
			throw new Exception('Upload Error: '.$_FILES["file"]["error"]);
			
		$filename=isset($_POST['savename']) ? $_POST['savename'] : date('YmdHis_').mt_rand(10000,99999);
		if(!strlen($filename) || preg_match('/[^-)(\w]/',$filename))
			throw new Exception("Invalid file name");
			
		$filename.='.'.end(explode('.',$_FILES['file']['name']));

		if(file_exists("../images/$filename"))
			throw new Exception("File '$filename' already exists,<br>try another file name.");
		if(!is_dir("../images"))
			if(!mkdir("../images",0770))
				throw new Exception("Cannot create directory 'images'");
				
		if(move_uploaded_file($_FILES["file"]["tmp_name"],"../images/$filename")){
			$imgtag="&lt;img src=\"../images/$filename\"&gt;";
		}else
			throw new Exception("Upload failed");
	}catch(Exception $e){
		$info=$e->getMessage();
	}
}else{
	$filename=date('YmdHis_').mt_rand(10000,99999);
	if(isset($_GET['id']))
		$filename=intval($_GET['id']);
}
?>
<!DOCTYPE html>
<html>
	<meta charset="utf-8">
	<title>Upload</title>
	<style type="text/css">
		body{
			background: #E0F0E0;
			font-size: 14px;
			overflow: hidden;
		}
		h3,h4{
			text-align: center;
		}
		a{
			color: #08C;
		}
	</style>
	<script type="text/javascript">
		function check_upload(){
			if(/\.(jpg|jpeg|png|gif|bmp|tif|tiff|ico|wmf)$/i.test(document.getElementById('file').value)){
				return true;
			}else{
				document.getElementById('info').innerHTML="Invalid image format";
			}
			return false;
		}
	</script>
	<body>
		<?php if(isset($imgtag)){ ?>
			<h3 style="margin:10px auto">Upload successfully.</h3>
			<div style="overflow:auto;white-space:nowrap">HTML Tag:<span style="color:red"><?php echo $imgtag ?></span></div>
			<p style="text-align:center"><a href="#" onclick="return window.close(),false;">Close</a></p>
		<?php }else if(isset($info)){ ?>
			<h4 style="margin:10px auto"><?php echo $info ?></h4>
			<a href="#" onclick="return history.back(),false;">&laquo;Go back</a>
		<?php }else{ ?>
			<form action="upload.php" method="post" enctype="multipart/form-data" onsubmit="return check_upload();">
				<input type="file" name="file" id="file">
				<div><span>File name: </span><input type="text" name="savename" value="<?php echo $filename?>" style="width:200px;"></div>
				<div style="text-align:center">
					<div id="info"> </div>
					<input type="submit" value="Upload"> 
				</div>
			</form>
		<?php } ?>
	</body>
</html>