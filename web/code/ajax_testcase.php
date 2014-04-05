<?php
session_start();
if(!isset($_SESSION['administrator']))
	die('Not administrator');
if(!isset($_SESSION['admin_tfa']) || !$_SESSION['admin_tfa'])
	die('No TFA');

function get_testcase_dir()
{
	$content = file_get_contents("http://127.0.0.1:8881/get_datapath");
	if(FALSE === $content)
		return FALSE;
	return $content.DIRECTORY_SEPARATOR;
}

function upload_file_handler($targetDir)
{
	@set_time_limit(250);
	umask(0007);

	// Create target dir
	if (!file_exists($targetDir)) {
		@mkdir($targetDir, 0770, true);
	}

	// Get a file name
	if (isset($_REQUEST["name"])) {
		$fileName = $_REQUEST["name"];
	} elseif (!empty($_FILES)) {
		$fileName = $_FILES["file"]["name"];
	} else {
		$fileName = uniqid("file_");
	}
	
	$fileName = basename($fileName);
	if($fileName=='' || $fileName=='.' || $fileName=='..' ){
		header($_SERVER['SERVER_PROTOCOL'] . ' 500 Internal Server Error', true, 500);
		die('{"jsonrpc" : "2.0", "error" : {"code": 104, "message": "Illegal file name."}, "id" : "id"}');
	}
	$filePath = $targetDir . DIRECTORY_SEPARATOR . $fileName;

	// Chunking might be enabled
	$chunk = isset($_REQUEST["chunk"]) ? intval($_REQUEST["chunk"]) : 0;
	$chunks = isset($_REQUEST["chunks"]) ? intval($_REQUEST["chunks"]) : 0;


	// Open temp file
	if (!$out = @fopen("{$filePath}.part", $chunks ? "ab" : "wb")) {
		header($_SERVER['SERVER_PROTOCOL'] . ' 500 Internal Server Error', true, 500);
		die('{"jsonrpc" : "2.0", "error" : {"code": 102, "message": "Failed to open output stream."}, "id" : "id"}');
	}

	if (!empty($_FILES)) {
		if ($_FILES["file"]["error"] || !is_uploaded_file($_FILES["file"]["tmp_name"])) {
			header($_SERVER['SERVER_PROTOCOL'] . ' 500 Internal Server Error', true, 500);
			die('{"jsonrpc" : "2.0", "error" : {"code": 103, "message": "Failed to move uploaded file."}, "id" : "id"}');
		}

		// Read binary input stream and append it to temp file
		if (!$in = @fopen($_FILES["file"]["tmp_name"], "rb")) {
			header($_SERVER['SERVER_PROTOCOL'] . ' 500 Internal Server Error', true, 500);
			die('{"jsonrpc" : "2.0", "error" : {"code": 101, "message": "Failed to open input stream."}, "id" : "id"}');
		}
	} else {	
		if (!$in = @fopen("php://input", "rb")) {
			header($_SERVER['SERVER_PROTOCOL'] . ' 500 Internal Server Error', true, 500);
			die('{"jsonrpc" : "2.0", "error" : {"code": 101, "message": "Failed to open input stream."}, "id" : "id"}');
		}
	}

	while ($buff = fread($in, 4096)) {
		fwrite($out, $buff);
	}

	@fclose($out);
	@fclose($in);

	// Check if file has been uploaded
	if (!$chunks || $chunk == $chunks - 1) {
		// Strip the temp .part suffix off 
		rename("{$filePath}.part", $filePath);
	}

	// Return Success JSON-RPC response
	die('{"jsonrpc" : "2.0", "result" : null, "id" : "id"}');

}

if(!isset($_SESSION['testcase_dir'])){
    $testcase_dir = get_testcase_dir();
    if(FALSE === $testcase_dir)
      die("Failed to get testcase directory");
    else
      $_SESSION['testcase_dir']=$testcase_dir;
}

if($_SERVER['REQUEST_METHOD']=='POST'){
	if(!isset($_REQUEST['problem_id']))
		die('{"jsonrpc" : "2.0", "error" : {"code": 104, "message": "No problem specified."}, "id" : "id"}');
	$prob_dir = $_SESSION['testcase_dir'].intval($_REQUEST['problem_id']);
	upload_file_handler($prob_dir);
}else{
	if(!isset($_GET['problem_id'],$_GET['op']))
		die('error');
	$prob_dir = $_SESSION['testcase_dir'].intval($_REQUEST['problem_id']);
	$op=$_GET['op'];
	if($op == 'list'){
		$arr = scandir($prob_dir);
		if(FALSE === $arr)
			$arr = array();
		echo '{"files":',json_encode(array_diff($arr,array('.','..'))),'}';
	}else if($op == 'del'){
		if(!isset($_GET['filename']))
			die('error');
		$name = basename($_GET['filename']);
		if($name!='.' && $name!='..' && $name!='' && unlink($prob_dir.DIRECTORY_SEPARATOR.$name))
			echo '__ok__';
		else
			echo 'failed';
	}else{
		die('invalid operation');
	}

}