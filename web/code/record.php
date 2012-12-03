<?php
require('inc/result_type.php');
require('inc/lang_conf.php');
session_start();
if(isset($_GET['start_id']))
  $start_id=intval($_GET['start_id']);
else
  $start_id=0;
if($start_id<0)
  die('Argument out of range.');

require('inc/database.php');

$cond="";
$user_id="";
$problem_id=0;
$result=-1;
$lang=-1;
$way="none";
$public_code=false;
$rank_mode=false;

if(isset($_GET['problem_id'])){
  $problem_id=intval($_GET['problem_id']);
}
if(isset($_GET['way'])){
  $way=$_GET['way'];
  if($way=="time"||$way=="memory"){
    $rank_mode=true;
    $result=0;
    if(!$problem_id)
      $problem_id=1000;
    $cond.=" and result=0";
  }
}
if(!$rank_mode) {
  if(isset($_GET['solution_id']))
    $cond.=" and solution_id=".intval($_GET['solution_id']);
  else {
    $row=mysql_fetch_row(mysql_query('select max(solution_id) from solution'));
    $top=0;
    if(!is_null($row[0]))
      $top=$row[0];
    $top-=$start_id;
    $cond.=" and solution_id<=$top";
  }
}
if($problem_id)
    $cond.=" and problem_id=$problem_id";
if(isset($_GET['user_id'])){
  $user_id=trim($_GET['user_id']);
  if(strlen($user_id))
    $cond.=' and user_id=\''.mysql_real_escape_string($user_id).'\'';
}
if($result==-1 && isset($_GET['result'])){
  $result=intval($_GET['result']);
  if($result!=-1)
    $cond.=" and result=$result";
}
if(isset($_GET['lang'])){
  $lang=intval($_GET['lang']);
  if($lang!=-1)
    $cond.=" and language=$lang";
}
if(isset($_GET['public'])){
  $public_code=true;
  $cond.=' and public_code';
}
$sql="";
if(strlen($cond))
  $sql="where".substr($cond, 4);

if($way=="time")
  $sql.=" order by time,memory";
else if($way=="memory")
  $sql.=" order by memory,time";
else
  $sql.=" order by solution_id desc";

if(!$rank_mode){
  $res=mysql_query("select solution_id,problem_id,user_id,result,score,time,memory,code_length,language,in_date,public_code from solution $sql limit 20");
}else{
  $res=mysql_query("select solution_id,problem_id,user_id,result,score,time,memory,code_length,language,in_date,public_code from solution $sql limit $start_id,20");
}
if($problem_id==0)
  $problem_id="";
?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Record</title>
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
      <div class="row-fluid">
        <div class="span12">
          <form action="record.php" method="get" class="form-inline" id="form_filter">
            <label>Problem:</label>
            <input type="text" class="input-mini" name="problem_id" id="ipt_problem_id" value="<?php echo $problem_id?>">
            <label>User:</label>
            <input type="text" class="input-small" name="user_id" id="ipt_user_id" value="<?php echo $user_id?>">
            <label>Result:</label>
            <select class="input-small" name="result" id="slt_result">
              <option value="-1">All</option>
              <?php foreach ($RESULT_TYPE as $type => $str)
                echo '<option value="',$type,'">',$str,'</option>';
              ?>
            </select>
            <label>Language:</label>
            <select class="input-small" name="lang" id="slt_lang">
              <option value="-1">All</option>
              <?php foreach ($LANG_NAME as $langid => $lang_name)
                echo '<option value="',$langid,'">',$lang_name,'</option>';
              ?>
            </select>
            <label>Sort:</label>
            <select class="input-small" name="way" id="slt_way">
              <option value="none">Submit</option>
              <option value="time">Time</option>
              <option value="memory">Memory</option>
            </select>
            <input type="hidden" name="start_id" value="0">
            <label style="display:inline-block;"><input <?php if($public_code)echo 'checked'?> style="margin:0 3px" id="chk_public" type="checkbox" name="public">Open Source</label>
            <span style="margin-left:5px" class="btn" id="btn_reset">Reset</span>
          </form>
        </div>
      </div>
      <div class="row-fluid">
        <div class="span12">

            <table class="table table-hover table-bordered" style="margin-bottom:0">
              <thead><tr>
                <th style="width:6%">ID</th>
                <th style="width:7%">Problem</th>
                <th style="width:12%">User</th>
                <th style="width:11%">Result</th>
                <th style="width:7%">Score</th>
                <th style="width:10%">Time</th>
                <th style="width:10%">Memory</th>
                <th style="width:8%">Length</th>
                <th style="width:11%">Language</th>
                <th style="width:17%">Submit Date</th>
              </tr></thead>
              <tbody id="tab_record">
                <?php 
                while($row=mysql_fetch_row($res)){
                  echo '<tr><td>',$row[0],'</td>';
                  echo '<td><a href="problempage.php?problem_id=',$row[1],'">',$row[1],'</a></td>';
                  echo '<td><a href="#uid">',$row[2],'</a></td>';
                  echo '<td><span class="label ',$RESULT_STYLE[$row[3]],'">',$RESULT_TYPE[$row[3]],'</span></td>';
                  echo '<td>',$row[4],'</td>';
                  if($row[3])
                    echo '<td></td><td></td>';
                  else{
                    echo '<td>',$row[5],' ms</td>';
                    echo '<td>',$row[6],' KB</td>';
                  }
                  echo '<td>',round($row[7]/1024,2),' KB</td>';
                  echo '<td><a target="_blank" href="sourcecode.php?solution_id=',$row[0],'">',$LANG_NAME[$row[8]],'</a>';
                  echo ' [<a href="#sw_open_',$row[0],'" class=',($row[10] ? '"a-green">O' : '"a-red">C'),'</a>]</td>';
                  echo '<td>',$row[9],'</td>';
                  echo '</tr>';
                }
                ?>
              </tbody>
            </table>
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
       
      <div class="modal hide" id="UserModal">
        <div class="modal-header">
          <a class="close" data-dismiss="modal">×</a>
          <h4>User Infomation</h4>
        </div>
        <div class="modal-body" id="user_status" style="max-height:350px">
          <p>Information isn't available.</p>
        </div>
        <div class="modal-footer">
          <a href="#" class="btn" data-dismiss="modal">Close</a>
        </div>
      </div>
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
        var cur=<?php echo $start_id?>;
        var now=window.location.search;
        var newuri=now.replace(/start_id=[^&]*&?/,'').replace(/&*$/,'');
        if(newuri=='')newuri='?';
        if(newuri!='?')newuri+='&';

        $('#slt_lang>option[value=<?php echo $lang;?>]').attr('selected','selected');
        $('#slt_result>option[value=<?php echo $result?>]').attr('selected','selected');
        $('#slt_way>option[value="<?php echo $way?>"]').attr('selected','selected');
        $('#nav_record').parent().addClass('active');
        $('#ret_url').val("record.php"+now);
        $('#btn-next').click(function(){
          location.href='record.php'+newuri+'start_id='+(cur+20);
          return false;
        });
        $('#btn-pre').click(function(){
          if(cur-20>=0)
            location.href='record.php'+newuri+'start_id='+(cur-20);
          return false;
        });
        function toggle_s(obj){
          if(obj.hasClass('a-red')){
            obj.removeClass('a-red');
            obj.addClass('a-green');
            obj.html('O');
          }else{
            obj.removeClass('a-green');
            obj.addClass('a-red');
            obj.html('C');
          }
        }
        $('#tab_record').click(function(E){
          var $target=$(E.target);
          if(!$target.is('a'))
            return true;
          var h=$target.attr('href');
          if(h.substr(0,9)=='#sw_open_'){
            $.ajax({
              type:"POST",
              url:"ajax_opensource.php",
              data:{"id":$target.attr('href').substr(9)},
              success:function(msg){if(/success/.test(msg))toggle_s($target);}
            });
            return false;
          }else if(h=='#uid'){
            $('#user_status').html("<p>Loading...</p>").load("ajax_user.php?user_id="+E.target.innerHTML).scrollTop(0);
            var win=$('#UserModal');
            win.children('.modal-header').children('h4').html('User Infomation');
            win.modal('show');
            return false;
          }
        });
        function fun_submit(){$('#form_filter').submit();}
        $('#slt_result').change(function(){
          $('#slt_way').val('none');
          fun_submit();
        });
        $('#slt_lang').change(fun_submit);
        $('#slt_way').change(fun_submit);
        $('#chk_public').change(fun_submit);
        $('#ipt_problem_id').keydown(function(E){
          if(E.keyCode==13)fun_submit();
        });
        $('#ipt_user_id').keydown(function(E){
          if(E.keyCode==13)fun_submit();
        });
        $('#btn_reset').click(function(){window.location="record.php?problem_id="+$("#ipt_problem_id").val()+"&user_id="+$("#ipt_user_id").val();});
      }); 
    </script>
  </body>
</html>
