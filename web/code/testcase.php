<?php 
if(!isset($_GET['problem_id']))
    die('Wrong argument.');
$prob_id=intval($_GET['problem_id']);

require('inc/checklogin.php');

if(!isset($_SESSION['user'],$_SESSION['administrator']))
  die("Permission denied");
if(!isset($_SESSION['admin_tfa']) || !$_SESSION['admin_tfa']){
  $_SESSION['admin_retpage'] = "testcase.php?problem_id=$prob_id";
  header("Location: admin_auth.php");
  exit;
}

require('inc/database.php');

$result=mysql_query("select title from problem where problem_id=$prob_id");
$row=mysql_fetch_row($result);
if(!$row)
  die('No such problem.');


$Title="Test Cases of $prob_id";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>

  <body>
    <?php require('page_header.php'); ?>  
          
    <div class="container-fluid">
        <div class="row-fluid center">
          <h3>Test cases of <?php echo $prob_id,' ',$row[0]; ?></h3>
        </div>
        <div class="row-fluid">
          <div class="span10 offset1" style="margin-bottom:10px">
            <span class="btn btn-primary" id="btn_refresh">
              <i class="icon-refresh icon-spin"></i> Refresh</span>
            <a class="btn btn-info" id="btn_uploader" data-toggle="modal">
              <i class="icon-upload-alt"></i> Upload</a>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span10 offset1 hide" id="uploader_wrap" style="margin-bottom:10px">
            <div id="html5_uploader" >Your browser doesn't support native upload.</div>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span10 offset1">
            <table class="table table-striped table-condensed" style="border-bottom: 1px solid #DDD;">
                <tbody id="file_list"></tbody>
            </table>
          </div>
        </div>
      <hr>
      <footer>
        <p>&copy; 2012-2014 Bashu Middle School</p>
      </footer>

    </div>

    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="../assets/js/jquery-ui.min.js"></script>
    <script src="../assets/js/plupload.full.min.js"></script>
    <script src="../assets/js/jquery.ui.plupload.min.js"></script>

    <script src="common.js"></script>
    <script type="text/javascript"> 
      var problem_id = <?php echo $prob_id?>;
      var uploader;
      LoadCSS('../assets/css/jquery-ui.min.css')
      LoadCSS('../assets/css/jquery.ui.plupload.css');
      function refresh_list()
      {
        $('#btn_refresh>i').addClass('icon-spin');
        $.getJSON('ajax_testcase.php?op=list&problem_id='+problem_id, function(obj){
          if(obj.files){
            var $list = $('#file_list').html('');
            $.each(obj.files,function(index,val){
              $list.append('<tr><td style="text-align:left">'+htmlEncode(val)+'</td><td><a href="#" class="text-error"><i class="icon icon-remove"></i></a></td></tr>');
            });
            $('#btn_refresh>i').removeClass('icon-spin');
          }
        })
      }
      $(document).ready(function(){
        $('#ret_url').val("testcase.php?problem_id="+problem_id);
        refresh_list();
        $('#btn_refresh').click(function(){
          refresh_list();
        });
        $('#btn_uploader').click(function(){
          $('#uploader_wrap').slideDown();
          return false;
        });
        $('#file_list').click(function(E){
          var $obj = $(E.target);
          if($obj.is('i')){
            var name = $obj.parent().parent().prev().text();
            if(!window.confirm("Are you sure to delete "+name))
                return false;
            $.get('ajax_testcase.php'+BuildUrlParms({'op':'del','problem_id':problem_id,'filename':name}), function(r){
              if(/__ok__/.test(r))
                refresh_list();
            });
            return false;
          }
        })
      });
      uploader=$("#html5_uploader").plupload({
        runtimes : 'html5',
        url : 'ajax_testcase.php',
        chunk_size : '10mb',
        multipart : true,
        multipart_params: {
          "problem_id": problem_id
        },
        max_retries: 2,
        complete:function(){
          refresh_list();
          $('#uploader_wrap').slideUp();
        }
        // filters : {
        //  max_file_size : '10mb',
        //  mime_types: [
        //    {title : "Image files", extensions : "jpg,gif,png"},
        //    {title : "Zip files", extensions : "zip"}
        //  ]
        // }
      });
    </script>
  </body>
</html>