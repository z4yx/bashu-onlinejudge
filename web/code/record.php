<?php
require('inc/result_type.php');
require('inc/lang_conf.php');
require('inc/checklogin.php');
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
if(isset($_GET['way']) && !preg_match('/\W/',$_GET['way'])){
  $way=$_GET['way'];
  if($way=="time"||$way=="memory"){
    $rank_mode=true;
    $result=0;
    if(!$problem_id)
      $problem_id=1000;
    $cond.=" and result=0";
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
if(!$rank_mode){
  $filter=$cond;
  if(isset($_GET['solution_id']))
    $cond=" and solution_id<=".intval($_GET['solution_id']).$cond;
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
  if(isset($_GET['start_id'])){
    $start_id=intval($_GET['start_id']);
    if($start_id<0)
      $start_id=0;
  }else
    $start_id=0;
  $res=mysql_query("select solution_id,problem_id,user_id,result,score,time,memory,code_length,language,in_date,public_code from solution $sql limit $start_id,20");
}
if($problem_id==0)
  $problem_id="";

$max_solution=0;
$min_solution=2100000000;
$num=mysql_num_rows($res);

function get_next_link()
{
  global $rank_mode,$min_solution,$num;
  parse_str($_SERVER["QUERY_STRING"],$arr); 
  if($rank_mode){
    global $start_id;
    $arr['start_id']=($num ? $start_id+20 : $start_id);
  }else{
    if($num)
      $arr['solution_id']=$min_solution-1;
  }
  return http_build_query($arr);
}
function get_pre_link()
{
  global $rank_mode,$max_solution;
  parse_str($_SERVER["QUERY_STRING"],$arr); 
  if($rank_mode){
    global $start_id;
    $arr['start_id']=($start_id>=20 ? $start_id-20 : 0);
  }else{
    global $filter;
    $sql="select solution_id from solution where solution_id>$max_solution $filter order by solution_id limit 20";
    $res=mysql_query($sql);
    $num=mysql_num_rows($res);
    if($num==0)
      $arr['solution_id']=$max_solution;
    else{
      while(--$num)
        mysql_fetch_row($res);
      $row=mysql_fetch_row($res);
      $arr['solution_id']=$row[0];
    }
  }
  return http_build_query($arr); 
}
$Title="Record";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>

  <body>
    <?php require('page_header.php'); ?>
    <div class="container-fluid">
      <div class="row-fluid">
        <div class="span12">
          <form action="record.php" method="get" class="form-inline" id="form_filter">
            <label>Problem:</label>
            <input type="text" class="input-mini" name="problem_id" id="ipt_problem_id" value="<?php echo $problem_id?>">
            <label>User:</label>
            <?php if(isset($_SESSION['user'])) echo'<div class="input-append">' ?>
            <input type="text" class="input-small" name="user_id" id="ipt_user_id" value="<?php echo $user_id?>">
            <?php if(isset($_SESSION['user'])) echo'<button class="btn" id="filter_me" data-myuid="',$_SESSION['user'],'" type="button">Me</button></div>' ?>
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
            <label class="checkbox">
              <input <?php if($public_code)echo 'checked'?> id="chk_public" type="checkbox" name="public">Open Source
            </label>
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
                  if($row[0]<$min_solution)
                    $min_solution=$row[0];
                  if($row[0]>$max_solution)
                    $max_solution=$row[0];
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
                  echo ' <a href="#sw_open_',$row[0],'"><i class=', ($row[10] ? '"icon-eye-open text-success"' : '"icon-eye-close muted"'), '></i></a> </td>';
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
            <a class="pager-pre-link shortcut-hint" title="Alt+A" href="record.php?<?php echo htmlspecialchars(get_pre_link())?>" id="btn-pre"><i class="icon-angle-left"></i> Previous</a>
          </li>
          <li>
            <a class="pager-next-link shortcut-hint" title="Alt+D" href="record.php?<?php echo htmlspecialchars(get_next_link())?>" id="btn-next">Next <i class="icon-angle-right"></i></a>
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
      <footer>
        <p>&copy; 2012-2014 Bashu Middle School</p>
      </footer>

    </div><!--/.container-->
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="../assets/js/common.js"></script>

    <script type="text/javascript"> 
      $(document).ready(function(){
        $('#slt_lang>option[value=<?php echo $lang;?>]').prop('selected',true);
        $('#slt_result>option[value=<?php echo $result?>]').prop('selected',true);
        $('#slt_way>option[value="<?php echo $way?>"]').prop('selected',true);
        $('#nav_record').parent().addClass('active');
        $('#ret_url').val("record.php"+window.location.search);

        function toggle_s(obj){
          if(obj.hasClass('icon-eye-close')){
            obj.removeClass('icon-eye-close');
            obj.addClass('icon-eye-open');
            obj.removeClass('muted');
            obj.addClass('text-success');
          }else{
            obj.removeClass('icon-eye-open');
            obj.addClass('icon-eye-close');
            obj.removeClass('text-success');
            obj.addClass('muted');
          }
        }
        $('#tab_record').click(function(E){
          var $target=$(E.target);
          if(!$target.is('a')){
            $target=$target.parent();
            if(!$target || !$target.is('a'))
              return;
          }
          var h=$target.attr('href');
          if(h.substr(0,9)=='#sw_open_'){
            $.ajax({
              type:"POST",
              url:"ajax_opensource.php",
              data:{"id":$target.attr('href').substr(9)},
              success:function(msg){if(/success/.test(msg))toggle_s($target.find('i'));}
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
        $('#filter_me').click(function(E){
          $('#ipt_user_id').val($(this).data('myuid'));
          fun_submit();
        })
        $('#btn_reset').click(function(){window.location="record.php?problem_id="+$("#ipt_problem_id").val()+"&user_id="+$("#ipt_user_id").val();});
      }); 
    </script>
  </body>
</html>
