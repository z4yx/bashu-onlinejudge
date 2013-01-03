<?php 
require('inc/result_type.php');
require('inc/lang_conf.php');
require('inc/checklogin.php');

if(isset($_GET['problem_id']))
  $prob_id=intval($_GET['problem_id']);
else if(isset($_SESSION['view']))
  $prob_id=$_SESSION['view'];
else
  $prob_id=1000;
require('inc/database.php');

$query="select title,description,input,output,sample_input,sample_output,hint,source,case_time_limit,memory_limit,case_score,defunct,has_tex from problem where problem_id=$prob_id";
$result=mysql_query($query);
$row=mysql_fetch_row($result);
if(!$row)
  die('Wrong Problem ID.');

?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Problem <?php echo $prob_id;?></title>
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
    <?php if($row[12])require('inc/mathjax_head.php') ?>
  </head>

  <body style="min-width:980px;">
    <?php 
      require('page_header.php');
      if($row[11]=='Y' && !isset($_SESSION['administrator']))
        $forbidden=true;
      else{
        $forbidden=false;
        $_SESSION['view']=$prob_id;

        if(isset($_SESSION['user'])){
          $query='select min(result) from solution where user_id=\''.$_SESSION['user']."' and problem_id=$prob_id group by problem_id";
          $user_status=mysql_query($query);
          if(mysql_num_rows($user_status)==0)
            $info = '<tr><td colspan="2" id="user_status_gray">Haven\'t submitted yet.</td></tr>';
          else{
            $statis=mysql_fetch_row($user_status);
            if($statis[0]==0){
              $info = '<tr><td colspan="2" id="user_status_green"><i class="icon-ok icon-white"></i> Congratulation !</td></tr>';
            }else{
              $info = '<tr><td colspan="2" id="user_status_red"><i class="icon-remove icon-white"></i> Try Again !</td></tr>';
            }
          }
        }else{
          $info = '<tr><td colspan="2" id="user_status_gray">Not logged in.</td></tr>';
        } 
        $result=mysql_query("select submit_user,solved,submit from problem where problem_id=$prob_id");
        $statis=mysql_fetch_row($result);
        $submit_user=$statis[0];
        $solved_user=$statis[1];
        $total_submit=$statis[2];

        $result=mysql_query("select result,count(*) as sum from solution where problem_id=$prob_id group by result");
        $arr=array();
        while($statis=mysql_fetch_row($result))
          $arr[$statis[0]]=$statis[1];
        ksort($arr);
      }
    ?>
    <div id="probdisp" class="container-fluid" style="font-size:16px">
      <?php 
      if($forbidden)
        echo '<div class="span12 center">Problem is not available!</div>';
      else{ ?>
      <div class="row-fluid">
        <div class="span9" id="leftside">
          <div style="margin-left: auto; margin-right: auto;text-align: center">
            <h2><?php echo $prob_id,' -- ',$row[0];if($row[11]=='Y')echo '<span style="vertical-align:middle" class="label label-important">Deleted</span>';?></h2>
          </div>
          <div class="row-fluid">
            <div class="span12">
              <h3>Description</h3>
              <div class="well well-small"><?php echo mb_ereg_replace('\r?\n','<br>',$row[1]);?></div>
            </div>
          </div>
          <div class="row-fluid">
            <div class="span12">
              <h3>Input</h3>
              <div class="well well-small"><?php echo mb_ereg_replace('\r?\n','<br>',$row[2]);?></div>
            </div>
          </div>
          <div class="row-fluid">
            <div class="span12">
              <h3>Output</h3>
              <div class="well well-small"><?php echo mb_ereg_replace('\r?\n','<br>',$row[3]);?></div>
            </div>
          </div>
          <div class="row-fluid">
            <div class="span12">
              <h3>Sample Input</h3>
              <div class="well well-small" style="font-family: monospace;"><?php echo mb_ereg_replace('\r?\n','<br>',$row[4]);?></div>
            </div>
          </div>
          <div class="row-fluid">
            <div class="span12">
              <h3>Sample Output</h3>
              <div class="well well-small" style="font-family: monospace;"><?php echo mb_ereg_replace('\r?\n','<br>',$row[5]);?></div>
            </div>
          </div>
          <?php if(strlen($row[6])){ ?>
          <div class="row-fluid">
            <div class="span12">
              <h3>Hint</h3>
              <div class="well well-small"><?php echo mb_ereg_replace('\r?\n','<br>',$row[6]);?></div>
            </div>
          </div>
          <?php } ?>
          <div class="row-fluid">
            <div class="span12">
              <h3>Source</h3>
              <div class="well well-small"><?php echo mb_ereg_replace('\r?\n','<br>',$row[7])?></div>
            </div>
          </div>
        </div>
        <div class="span3" style="font-size:14px;" id="rightside">
          <div class="row-fluid">
            <h2 style="text-align:right"><span id="btn_hide" class="btn btn-small">Hide Information &raquo;</span></h2>
          </div>
          <div class="row-fluid">
            <h3>&nbsp;</h3>
          </div>
          <div class="row-fluid">
            <div class="span12">
              <div class="well well-small">
                <table class="table table-condensed table-striped" style="margin-bottom:0px;">
                  <tbody>
                    <tr><td style="text-align:left">Case Time Limit:</td><td><?php echo $row[8]?> ms</td></tr>
                    <tr><td style="text-align:left">Memory Limit:</td><td><?php echo $row[9]?> KB</td></tr>
                    <tr><td style="text-align:left">Case score:</td><td><?php echo $row[10]?></td></tr>
                  </tbody>
                </table>
              </div>
            </div>
          </div>
          <div class="row-fluid"><div class="span12">
            <div id="status" class="well well-small" style="margin-top:10px">
              <table class="table table-condensed table-striped" style="margin-bottom:0px">
                <tbody>
                <?php echo $info ?>
                <tr><td style="text-align:left">Users Submitted:</td><td><?php echo $submit_user?></td></tr>
                <tr><td style="text-align:left">Users Solved:</td><td><?php echo $solved_user?></td></tr>
                <tr><td style="text-align:left">Total Submits:</td><td><?php echo $total_submit?></td></tr>
                <?php
                  foreach ($arr as $type => $cnt) {
                    if(isset($RESULT_TYPE[$type]))
                      echo '<tr><td style="text-align:left">',$RESULT_TYPE[$type],':</td><td>',$cnt,'</td></tr>';
                  }
                ?>
                </tbody>
              </table>
            </div>
          </div></div>
          <div class="row-fluid"><div class="span12" style="text-align: center;">
            <div id="function" class="well well-small" style="margin-top:10px">
              <a href="#" class="btn btn-primary" id="action_submit">Submit</a>
              <a href="record.php?way=time&amp;problem_id=<?php echo $prob_id?>" class="btn btn-info">Status</a>
              <a href="board.php?problem_id=<?php echo $prob_id;?>" class="btn btn-warning">Discuss</a>
            </div>
          </div></div>
          <?php if(isset($_SESSION['administrator'])){?>
          <div class="row-fluid">
            <div class="span12" style="text-align:center">
              <div class="well well-small" style="margin-top:10px">
                <a href="edit.php?problem_id=<?php echo $prob_id?>" class="btn btn-success">Edit</a>
                <span id="action_delete" class="btn btn-danger"><?php echo $row[11]=='N' ? 'Delete' : 'Resume';?></span>
              </div>
            </div>
          </div>
          <?php }?>
        </div>
        <?php }?>
      </div>
      <hr>
      <footer class="muted" style="text-align: center;font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>

    </div>

    <div class="modal hide" id="SubmitModal">
      <div class="modal-header">
        <a class="close" data-dismiss="modal">&times;</a>
        <h4>Submit solution</h4>
      </div>
      <form style="margin: 0" action="submit.php" method="post" id="form_submit">
        <div class="modal-body" style="padding-top:5px">
          <div style="clear:both">
            <h5 class="center">Paste your code here.</h5>
            <textarea style="width: 750px;" id="detail_input" rows="17" name="source"></textarea>
          </div>
          <div class="alert alert-error hide" id="submit_result"></div>
        </div>
        <div class="modal-footer">
          <div style="float:left">
              <label class="control-label" style="display:inline-block;" for="prob_input">Problem</label>
              <input type="text" class="input-mini" style="font-weight: bold;margin-bottom: 0;" id="prob_input" name="problem">
          </div>
          <label style="display:inline-block;"><input style="margin:0 3px" type="checkbox" name="public">Share this code</label>
          <select name="language" id="slt_lang">
            <?php foreach ($LANG_NAME as $langid => $lang) {
              echo '<option value="',$langid,'">',$lang,'</option>';
            } ?>
          </select>
          <input class="btn btn-primary" value="Submit" type="submit">
          <a href="#" class="btn" data-dismiss="modal">Close</a>
        </div>
      </form>
    </div>

    <div id="show_tool">
      <span id="btn_submit2" class="btn btn-mini btn-primary">Submit</span>
      <span id="btn_show" class="btn btn-mini btn-primary">&laquo; Show Information</span>
    </div>

    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="common.js"></script>

    <script type="text/javascript"> 
      $(document).ready(function(){
        var prob=<?php echo $prob_id?>;
        $('#nav_prob').parent().addClass('active');
        $('#ret_url').val("problempage.php?problem_id="+prob);
        $('#action_delete').click(function(){
          $.ajax({
            url:"ajax_deleteprob.php?problem_id="+prob,
            dataType:"html",
            success:function(){location.reload();}
          });
        });
        $('#form_submit').submit(function(){
          var code = $('#detail_input').val();
          if($.trim(code) == '' || code.length > 30000){
            $('#submit_result').html("Code is too short or too long.").show();
            return false;
          }else{
            $('#submit_result').hide();
            //$('#SubmitModal').modal('hide');
            return true;
          }
        });
        $('#action_submit').click(function(){
          <?php if(!isset($_SESSION['user'])){?>
            alert("You haven't logged in.");
          <?php }else{?>
            $('#prob_input').val(''+prob);
            $('#SubmitModal').modal('show');
          <?php }?>
          return false;
        });
        $('#btn_submit2').click(function(){$('#action_submit').click();});
        $('#btn_hide').click(function(){
          $('#rightside').hide();
          $('#leftside').addClass('span12').removeClass('span9');
          $('#show_tool').show();
        });
        $('#btn_show').click(function(){
          $('#leftside').addClass('span9').removeClass('span12');
          $('#rightside').show();
          $('#show_tool').hide();
        });
        <?php 
        if(isset($_SESSION['lang']))
          echo '$(\'#slt_lang>option[value=',$_SESSION['lang'],']\').attr("selected","selected");';
        ?>
      });
    </script>
  </body>
</html>
