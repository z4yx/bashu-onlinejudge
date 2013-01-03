<?php 
if(!isset($_GET['problem_id']))
   die('Wrong argument.');
$prob_id=intval($_GET['problem_id']);

require('inc/checklogin.php');
?>
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Edit problem <?php echo $prob_id;?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <link href="../assets/css/bootstrap.css" rel="stylesheet">
    <link href="../assets/css/bootstrap-responsive.css" rel="stylesheet">
    <link href="../assets/css/docs.css" rel="stylesheet">
    <style type="text/css">
      #edit_form span{
        vertical-align: middle;
      }
      input.error{
        color: #B94A48;
        border-color: #B94A48;
      }
      textarea, #edit_form input, #edit_form select{
        margin-bottom: 0;
      }
      textarea[rows="1"]{
        height: 18px;
      }
    </style>
    <script type="text/javascript" src="edit.js"></script>
    <!--[if IE 6]>
    <link href="ie6.min.css" rel="stylesheet">
    <![endif]-->
    <!--[if lt IE 9]>
    <script src="../assets/js/html5.js"></script>
    <![endif]-->
  </head>

  <body>
    <?php 
    require('page_header.php'); 
    if(!isset($_SESSION['user']) || !isset($_SESSION['administrator'])) {
      echo '<div style="text-align:center">You are not administrator</div>';
    }else {
      require('inc/database.php');

      $query="select title,description,input,output,sample_input,sample_output,hint,source,case_time_limit,memory_limit,case_score,compare_way from problem where problem_id=$prob_id";
      $result=mysql_query($query);
      $row=mysql_fetch_row($result);
      if(!$row)
        echo '<div style="text-align:center">Wrong Problem ID</div>';
      else { 
        $way='tra';
        $prec=-1;
        switch ($row[11] >> 16) {
          case 0:
            $way='tra';
            break;
          case 1:
            $way='float';
            $prec=($row[11] & 65535);
            break;
          case 2:
            $way='int';
            break;
          case 3:
            $way='spj';
            break;
        }

    ?>

    <div class="container-fluid" style="font-size:16px">
      <form action="editproblem.php" method="post" id="edit_form">
        <input type="hidden" name="op" value="edit">
        <input type="hidden" name="problem_id" value="<?php echo $prob_id?>">
        <div class="row-fluid">
          <div class="span5">
            <p>
              <span>Problem ID: </span> 
              <input type="text" disabled value="<?php echo $prob_id?>">
            </p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span9">
            <p><span>Title: </span><textarea class="span8" name="title" rows="1"><?php echo $row[0]?></textarea></p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span5">
            <p><span>Case time: </span><input id="input_time" name="time" class="input-mini" type="text" value="<?php echo $row[8]?>"><span> ms</span></p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span5">
            <p><span>Memory: </span><input id="input_memory" name="memory" class="input-mini" type="text" value="<?php echo $row[9]?>"><span> KB</span></p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span12">
            <p><span>Validator: </span>
              <select name="compare" id="input_cmp" style="width:auto">
                <option value="tra">Traditional</option>
                <option value="int">Integer</option>
                <option value="float">Real Number</option>
                <option value="spj">Special Judge</option>
              </select>
              <select name="precision" class="hide input-mini" id="input_cmp_pre"></select>
              <span id="input_cmp_help" class="help-inline"></span>
            </p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span5">
            <p><span>Case score: </span><input id="input_score" name="score" class="input-mini" type="text" value="<?php echo $row[10]?>"></p>
          </div>
        </div>      
        <div class="row-fluid">
          <div class="span9">
            <p>
              Description:<br>
              <textarea class="span12" name="description" rows="13"><?php echo htmlspecialchars($row[1])?></textarea>
            </p>
          </div>
        </div>       
        <div class="row-fluid">
          <div class="span9">
            <p>
              Input:<br>
              <textarea class="span12" name="input" rows="8"><?php echo htmlspecialchars($row[2])?></textarea>
            </p>
          </div>
        </div>       
        <div class="row-fluid">
          <div class="span9">
            <p>
              Output:<br>
              <textarea class="span12" name="output" rows="8"><?php echo htmlspecialchars($row[3])?></textarea>
            </p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span9">
            <p>
              Sample Input:<br>
              <textarea class="span12" name="sample_input" rows="8"><?php echo htmlspecialchars($row[4])?></textarea>
            </p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span9">
            <p>
              Sample Output:<br>
              <textarea class="span12" name="sample_output" rows="8"><?php echo htmlspecialchars($row[5])?></textarea>
            </p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span9">
            <p>
              Hint:<br>
              <textarea class="span12" name="hint" rows="8"><?php echo htmlspecialchars($row[6])?></textarea>
            </p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span9">
            <p>
              Source:<br>
              <textarea class="span12" name="source" rows="1"><?php echo htmlspecialchars($row[7])?></textarea>
            </p>
          </div>
        </div>
        <div class="row-fluid">
          <div class="span9" style="text-align:center">
            <input type="submit" class="btn btn-primary" value="Submit">
          </div>
        </div>
      </form>
      <hr>
      <footer class="muted" style="text-align: center;font-size:12px;">
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>
    </div>
    <div class="html-tools">
      <div class="backsqare" id="tools">
        <table class="table table-bordered table-condensed table-striped" style="width:100%">
          <caption>HTML Tools</caption>
          <thead>
            <tr>
              <th>Function</th>
              <th>Description</th>
            </tr>
          </thead>
          <tbody>
            <tr>
              <td><button class="btn btn-mini" id="tool_less">Less(&lt;)</button></td>
              <td>&amp;lt;</td>
            </tr>
            <tr>
              <td><button class="btn btn-mini" id="tool_greater">Greater(&gt;)</button></td>
              <td>&amp;gt;</td>
            </tr>
            <tr>
              <td><button class="btn btn-mini" id="tool_img">Image</button></td>
              <td>&lt;img src=&quot;...&quot;&gt;</td>
            </tr>
            <tr>
              <td><button class="btn btn-mini" id="tool_sup">Superscript</button></td>
              <td>&lt;sup&gt;...&lt;/sup&gt;</td>
            </tr>
            <tr>
              <td><button class="btn btn-mini" id="tool_sub">Subscript</button></td>
              <td>&lt;sub&gt;...&lt;/sub&gt;</td>
            </tr>
            <tr>
              <td><button class="btn btn-mini" id="tool_samp">Monospace</button></td>
              <td>&lt;samp&gt;...&lt;/samp&gt;</td>
            </tr>
            <tr>
              <td><button class="btn btn-mini" id="tool_inline">Inline TeX</button></td>
              <td>[inline]...[/inline]</td>
            </tr>
            <tr>
              <td><button class="btn btn-mini" id="tool_tex">TeX</button></td>
              <td>[tex]...[/tex]</td>
            </tr>
          </tbody>
        </table>
      </div>
      <div style="text-align:center;margin-top:10px">
        <button class="btn btn-info" id="btn_upload">Upload Image</button>
      </div>
    </div>

    <?php
      }
    }
    ?>


    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="common.js"></script>

    <script type="text/javascript"> 
      $(document).ready(function(){
        var pid=<?php echo $prob_id?>;
        var loffset=window.screenLeft+200;
        var toffset=window.screenTop+200;
        $('#ret_url').val("edit.php?problem_id="+pid);
        function show_help(way){
          if(way=='float'){
            $('#input_cmp_pre').show();
            $('#input_cmp_help').html('Output must ONLY contain real numbers.Please select precision.');
          }else{
            $('#input_cmp_pre').hide();
            if(way=='tra')
              $('#input_cmp_help').html('Generic comparsion.Trailing space is ignored.');
            else if(way=='int')
              $('#input_cmp_help').html('Output must ONLY contain integers.');
            else if(way=='spj')
              $('#input_cmp_help').html('Please make sure there is "spj.exe"(on windows) or "spj.cpp"(on linux) in your data folder.');
          }
        }
        $('#input_cmp>option[value="<?php echo $way?>"]').attr('selected','selected');
        (function(){
          var option='',k=<?php echo $prec?>;
          for(var i=0;i<10;i++){
            option+='<option';
            if(i==k)option+=' selected';
            option+=' value="'+i+'">'+i+'</option>';
          }
          $('#input_cmp_pre').html(option);
          show_help($('#input_cmp').val());
        })();
        $('#input_cmp').change(function(E){show_help($(E.target).val());});
        $('#btn_upload').click(function(){
          window.open("upload.php?id="+pid,'upload_win2','left='+loffset+',top='+toffset+',width=300,height=100,channelmode=yes,directories=no,toolbar=no,resizable=no,menubar=no,location=no');
        });
        $('#edit_form textarea').focus(function(e){cur=e.target;});
        $('#edit_form input').blur(function(e){
          e.target.value=$.trim(e.target.value);
          var o=$(e.target);
          if(!e.target.value||(/\D/.test(e.target.value)))
            o.addClass('error');
          else
            o.removeClass('error');
        });
        $('#edit_form').submit(function(){
          var str=$('#input_memory').val();
          if(!str||(/\D/.test(str))){
            window.location.hash='#edit_form';
            return false;
          }
          str=$('#input_time').val();
          if(!str||(/\D/.test(str))){
            window.location.hash='#edit_form';
            return false;
          }
          str=$('#input_score').val();
          if(!str||(/\D/.test(str))){
            window.location.hash='#edit_form';
            return false;
          }
          return true;
        });
        $('#tools').click(function(e){
          if(!($(e.target).is('button')))return false;
          if(typeof(cur)=='undefined')return false;
          var op=e.target.id;
          var slt=GetSelection(cur);
          if(op=="tool_greater")
            InsertString(cur,'&gt;');
          else if(op=="tool_less")
            InsertString(cur,'&lt;');
          else if(op=="tool_img"){
            var url=prompt("Please input image url.","");
            if(url){
              InsertString(cur,slt+'<img src="'+url+'">');
            }
          }else if(op=="tool_inline"||op=="tool_tex"){
            op=op.substr(5);
            InsertString(cur,'['+op+']'+slt+'[/'+op+']');
          }else{
            op=op.substr(5);
            InsertString(cur,'<'+op+'>'+slt+'</'+op+'>');
          }
          return false;
        });
      });
    </script>
  </body>
</html>
