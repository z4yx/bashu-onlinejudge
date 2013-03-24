<?php
require('inc/checklogin.php');

if(isset($_GET['start_id']))
  $query_id=intval($_GET['start_id']);
else
  $query_id=2100000000;

if(isset($_GET['problem_id'])){
  $cond_prob='and problem_id='.intval($_GET['problem_id']);
  $query_prob=substr($cond_prob, 4);
}else{
  $query_prob=$cond_prob='';
}
require('inc/database.php');
$subquery="select thread_id from message where thread_id<$query_id $cond_prob order by thread_id desc limit 50";
$res=mysql_query("select min(thread_id) from ($subquery) as tmptab");
if(!$res)
  die('Wrong argument');
$row=mysql_fetch_row($res);
$range=$row[0];

function get_pre_link($top)
{
  global $cond_prob;
  $res=mysql_query("select max(thread_id) from (select thread_id from message where thread_id>=$top $cond_prob order by thread_id limit 50) as tmptab");
  $row=mysql_fetch_row($res);
  if($row[0])
    $pre=$row[0]+1;
  else
    $pre=2100000000;
  return $pre;
}

$Title="Web Board";
?>
<!DOCTYPE html>
<html>
  <?php require('head.php'); ?>
  <body>
    <script type="text/x-mathjax-config">
    MathJax.Hub.Config({
      skipStartupTypeset:true
    });
    </script>
    <?php require('inc/mathjax_head.php');?>

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
            <div id="PreviewPopover" class="popover left" >
              <div class="arrow"></div>
              <div class="popover-inner">
                <h3 class="popover-title">Preview<a class="close">×</a></h3>
                <div class="popover-content">
                    <div id="preview_content"></div>
                </div>
              </div>
            </div>
            <div style="float:left">
              <span  style="margin-left: 60px;" id="post_preview" class="btn btn-info">Preview</span>
            </div>
            <div style="float:right">
              <input id="post_input" type="submit" class="btn btn-primary" value="Post">
              <button id="cancel_input" class="btn">Cancel</button>
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
          <a href="#" title="Alt+N" class="btn btn-primary btn-small" id="new_msg" style="margin-left:25px"><i class="icon-file"></i> Post New Message</a>
          <?php
            $top=$query_id;
            if($range){
              $res=mysql_query("select title,depth,user_id,message_id,in_date,thread_id,problem_id,ASCII(content) from message where thread_id<$query_id and thread_id>=$range $cond_prob order by thread_id desc,orderNum");
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
                if($row[3]==$row[5] && $deep>0)
                  echo '&nbsp;<span class="label label-warning">latest</span>';
                if($deep==0 && $row[6])
                    echo '&nbsp;&nbsp;<a class="prob_link" href="problempage.php?problem_id=',$row[6],'">Problem ',$row[6],'</a>';
                echo ' <button id="reply_msg',$row[3],'" class="btn btn-mini">Reply</button>';
                if($row[7])
                  echo '<p class="msg_content msg_detailed">';
                else
                  echo '<p class="msg_content">';
                echo '<a class="msg_link" href="ajax_message.php?message_id=',$row[3],'" id="msg',$row[3],'">',htmlspecialchars($row[0]),'</a>';
                echo '</p></div></div>';
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
            <a class="pager-pre-link" title="Alt+A" href="board.php?<?php echo $query_prob,'&amp;start_id=',get_pre_link($top) ?>" id="btn-pre"> <i class="icon-angle-left"></i> Newer</a>
          </li>
          <li>
            <a class="pager-next-link" title="Alt+D" href="<?php if($range) echo 'board.php?',$query_prob,'&amp;start_id=',$range; ?>#" id="btn-next">Older <i class="icon-angle-right"></i></a>
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
        $('#nav_bbs').parent().addClass('active');
        $('#ret_url').val('board.php?<?php echo $query_prob,"&start_id=",$query_id?>');

        function dealwithlinks($jqobj)
        {
          $jqobj.find('a').each(function(){
            var Href = this.getAttribute("href",2);
            Href=Href.replace(/^([ \t\n\r]*javascript:)+/i,'');
            if(!(/(ht|f)tps?:\/\//i.test(Href)))
              Href = "http://"+Href;
            this.href=Href;
          });
        }
        $('#comments').click(function(E){
          if(! $(E.target).is("a.msg_link"))
            return;
          var ID=E.target.id+'_detail';
          var node=document.getElementById(ID);
          var p=$(E.target).parent();
          if(node){
            $(node).remove();
            p.removeClass("expanded");
          }else{
            if(p.hasClass("msg_detailed")){
              p.addClass("expanded");
              p.after('<pre id="'+ID+'"><div id="'+ID+'_div"></div></pre>');
              $.get('ajax_message.php?message_id='+E.target.id.substring(3),function(data){
                dealwithlinks( $('#'+ID+'_div').html(parseBBCode(data)) );
                MathJax.Hub.Queue(["Typeset",MathJax.Hub,(ID+'_div')]);
              });
            }else{
              var tmp=$('#alert_nothing').show();
              setTimeout(function(){tmp.fadeOut(400);},1000);
            }
          }
          return false;
        });
        var detail_ele=document.getElementById('detail_input');
        var minW=260,minH=100;
        function open_replypanel(msg_id){
          <?php if(isset($_SESSION['user'])){?>
          var title = ((msg_id=='0')?'Post New Message':'Reply for #'+msg_id);
          $('#msgid_input').val(msg_id);
          $('#replypanel h4').html(title);
          $('#replypanel').show();
          $('#msg_input').focus();
          <?php }else{echo 'alert("Please login first");';}?>
          return false;
        }
        $('#new_msg').click(function(){open_replypanel('0')});
        $('#comments button').click(function(E){open_replypanel(E.target.id.substring(9))});
        reg_hotkey(78,function(){$('#new_msg').click()}); //Alt+N

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
        reg_hotkey(83,function(){$('#replypanel form').submit()}); //Alt+S

        $('#cancel_input').click(function(){
          $('#replypanel').hide();
          return false;
        });
        $('#replypanel').keyup(function(E){
          E.which==27 && $('#replypanel').hide();
        });
        $('#post_preview').click(function(){
          var data=$('#detail_input').val();
          dealwithlinks( $('#preview_content').html(parseBBCode(data)));
          $('#PreviewPopover').show();
          MathJax.Hub.Queue(["Typeset",MathJax.Hub,('preview_content')]);
        });
        $('#PreviewPopover a.close').click(function(){
          $('#PreviewPopover').hide();
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
