<?php
require('inc/checklogin.php');

if(isset($_GET['start_id']))
  $top=intval($_GET['start_id']);
else
  $top=2100000000;
if(isset($_GET['problem_id']))
  $cond_prob='and problem_id='.intval($_GET['problem_id']);
else
  $cond_prob='';

require('inc/database.php');
$subquery="select thread_id from message where thread_id<$top $cond_prob order by thread_id desc limit 50";
$res=mysql_query("select min(thread_id) from ($subquery) as tmptab");
if(!$res)
  die('Wrong argument');
$row=mysql_fetch_row($res);
$range=$row[0];

?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Web Board</title>
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
    <div class="replypanel" id="replypanel">
      <div class="backsqare" style="background-color: #CCCCCC;">
        <h4 style="text-align:center;margin-bottom:10px;">Post New Message</h4>
        <form class="form-horizontal" method="post" action="postmessage.php">
          <fieldset>
            <div class="control-group">
              <label class="control-label" for="msg_input">Message</label>
              <div class="controls">
                <input type="text" class="input-xxlarge" id="msg_input" name="message">
              </div>
            </div>
            <div class="control-group">
              <label class="control-label" for="detail_input">Detail</label>
              <div class="controls">
                <textarea class="input-xxlarge" id="detail_input" rows="7" name="detail"></textarea>
              </div>
            </div>
            <div style="float:right">
              <input id="post_input" type="submit" class="btn btn-primary" value="Post">
              <span id="cancel_input" class="btn">Cancel</span>
            </div>
            <div id="post_status"></div>
          </fieldset>
          <input type="hidden" name="message_id" id="msgid_input">
          <?php if(isset($_GET['problem_id'])){
            echo '<input type="hidden" name="problem_id" value="',$_GET['problem_id'],'">';
          }?>
        </form>
        <div class="resize_sym" id="resize"></div>   
      </div>
    </div>
    <div class="alert hide center nocontent" id="alert_nothing">No content in this message!</div>
    <div class="container-fluid">
      <div class="row-fluid">
        <div class="span12" id="comments">
          <a href="#" class="btn btn-primary btn-small" id="new_msg" style="margin-left:25px">Post New Message</a>
          <?php 
            if($range){
              $res=mysql_query("select title,depth,user_id,message_id,in_date,thread_id,problem_id,ASCII(content) from message where thread_id<$top and thread_id>=$range $cond_prob order by thread_id desc,orderNum");
              $deep=-1;
              $top=0;
              $cnt=0;
              while($row=mysql_fetch_row($res)){
                if($row[1]>$deep){
                  if($deep>-1)
                    echo '<ul>';
                }else{
                  echo '</li>';
                  while($deep>$row[1]){
                    $deep--;
                    echo '</ul></li>';
                  }
                  if($row[1]==0)
                    echo '</ul>';
                }
                $deep=$row[1];
                if($row[5]>$top)
                  $top=$row[5];
                if($deep==0)
                  echo '<hr><ul>';
                echo '<li class="comment">';
                if((++$cnt)&1)
                  echo '<div class="msg msg_odd">';
                else
                  echo '<div class="msg msg_even">';
                echo '<div class="msg_container"><strong>',$row[2],'</strong> ',$row[4];
                if($deep==0 && $row[6])
                    echo '&nbsp;&nbsp;<a class="prob_link" href="problempage.php?problem_id=',$row[6],'">Problem ',$row[6],'</a>';
                echo ' <button id="reply_msg',$row[3],'" class="btn btn-mini">Reply</button><p>';
                if($row[7])
                  echo '<span>+ </span>';
                echo '<a href="#" id="msg',$row[3],'">',htmlspecialchars($row[0]),'</a></p></div></div>';
              }
              echo '</li>';
              while($deep>0){
                $deep--;
                echo '</ul></li>';
              }
              echo '</ul>';
              $top++;
            }else
              echo '<h4 style="text-align: center;">No Message Here!</h4>';
          ?> 
        </div> 
      </div>
      <div class="row-fluid">
        <ul class="pager">
          <li>
            <a href="#" id="btn-pre">&larr; Newer</a>
          </li>
          <li>
            <a href="#" id="btn-next">Older &rarr;</a>
          </li>
        </ul>
      </div> 
      
      <hr>
      <footer class="muted" style="text-align: center;font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>

    </div>
    <script src="../assets/js/bbcode.js"></script>
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="common.js"></script>

    <script type="text/javascript"> 
      $(document).ready(function(){
        var cur=<?php echo $top?>;
        var prob_id="<?php if(isset($_GET['problem_id'])) echo 'problem_id=',$_GET['problem_id'],'&';?>";
        $('#nav_bbs').parent().addClass('active');
        $('#ret_url').val('board.php?'+prob_id+'start_id='+cur);
        <?php 
          $subquery="select thread_id from message where thread_id>=$top $cond_prob order by thread_id limit 50";
          $res=mysql_query("select max(thread_id) from ($subquery) as tmptab");
          $row=mysql_fetch_row($res);
          if($row[0])
            $pre=$row[0]+1;
          else
            $pre=2100000000;
        ?>
        $('#btn-next').click(function(){
          <?php if($range){?>
          location.href='board.php?'+prob_id+'start_id='+<?php echo $range?>;
          <?php }?>
          return false;
        });
        $('#btn-pre').click(function(){
          location.href='board.php?'+prob_id+'start_id='+<?php echo $pre?>;
          return false;
        });
        $('#comments p>a').click(function(E){
          var ID=E.target.id+'_detail';
          var node=document.getElementById(ID);
          var a=$(E.target);
          if(node){
            $(node).remove();
            a.prev('span').html('+ ');
          }else{
            var sp=a.prev('span');
            if(sp.length){
              sp.html('- ');
              a.parent().after('<pre id="'+ID+'"></pre>');
              $.get('ajax_message.php?message_id='+E.target.id.substring(3),function(data){
                document.getElementById(ID).innerHTML=parseBBCode(data);
              });
            }else{
              var tmp=$('#alert_nothing').show();
              setTimeout(function(){tmp.fadeOut(400);},1000);
            }
          }
          return false;
        });
        var detail_ele=document.getElementById('detail_input');
        var minW=150,minH=100;
        $('#new_msg').click(function(){
          <?php if(isset($_SESSION['user'])){?>
          $('#msgid_input').val('0');
          $('#replypanel h4').html('Post New Message');
          $('#replypanel').show();
          <?php }else{echo 'alert("Please login first");';}?>
          return false;
        });
        $('#comments button').click(function(E){
          <?php if(isset($_SESSION['user'])){?>
          var ID=E.target.id.substring(9);
          $('#msgid_input').val(ID);
          $('#replypanel h4').html('Reply for #'+ID);
          $('#replypanel').show();
          <?php }else{echo 'alert("Please login first");';}?>
          return false;
        });
        $('#replypanel form').submit(function(){
          var msg=$.trim($('#msg_input').val());
          if(msg.length==0){
            $('#post_status').html('Message can\'t be empty');
            return false;
          }
          if(msg.length>150){
            $('#post_status').html('Message is too long');
            return false;
          }
          $('#post_status').html('');
          return true;
        });
        $('#cancel_input').click(function(){
          $('#replypanel').hide();
          return false;
        });
        function move_handle(E){
          var w=origX-E.clientX+origW;
          var h=E.clientY-origY+origH;
          if(w>=minW){
            $(detail_ele).width(w);
            $('#msg_input').width(w);
          }
          if(h>=minH)
            $(detail_ele).height(h);
        }
        $('body').mouseup(function(){
          $('body').unbind('mousemove');
        });
        $('#resize').mousedown(function(E){
          origX=parseInt(E.clientX);
          origY=parseInt(E.clientY);
          origW=$(detail_ele).width();
          origH=$(detail_ele).height();
          $('body').unbind('mousemove').mousemove(move_handle);
          return false;
        });
        $('body').mouseleave(function(){$('body').unbind('mousemove');});
      }); 
    </script>
  </body>
</html>
