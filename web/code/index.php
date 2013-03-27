<?php 
function encode_user_id($user)
{
  if(!extension_loaded('openssl'))
    return false;
  $iv='7284565820000000';
  $key=hash('sha256','my)(password_xx0',true);
  return openssl_encrypt($user,'aes-256-cbc',$key,false,$iv);
}
require('inc/checklogin.php');
require('inc/database.php');
$res=mysql_query("select content from news where news_id=0");
$index_text=($row=mysql_fetch_row($res)) ? $row[0] : '';
$Title="Welcome to Bashu OnlineJudge";
?>
<!DOCTYPE html>
<html>

    <?php require('head.php'); ?>

  <body>
    <?php require('page_header.php'); ?>  
          
    <div class="container-fluid">
      <div class="row-fluid">
        <div class="span10 offset1">
          <div id="newspad" class="well" style="font-size:16px;padding:19px 40px; background-color: #FFF">
            <div id="title" style="text-align:center; font-size:24px;cursor: pointer;">
                <h1 style="padding-bottom: 10px;">Welcome to Bashu OnlineJudge <i class="icon-double-angle-down"></i></h1>
            </div> 
            <div id="mainarea" style="display: none;">
                <?php echo $index_text?>
            </div>
          </div>
        </div>
      </div>
      <div class="row-fluid">
        <div id="chart" class="span12" style="height: 400px; margin: 0 auto"></div>
      </div>
      <hr>
      <footer>
        <p>&copy; 2012 Bashu Middle School</p>
      </footer>
    </div>
    <script type="text/javascript">
      <?php
        echo 'var ws_url="ws://',$_SERVER["SERVER_ADDR"],':6844/";';
        if(isset($_SESSION['user']))
          echo 'var userid="',encode_user_id('id-'.$_SESSION['user']),'";';
      ?>
    </script>
    <script src="../assets/js/jquery.js"></script>
    <script src="../assets/js/bootstrap.min.js"></script>
    <script src="../assets/js/highcharts.js"></script>
    <script src="common.js"></script>
    <script src="../assets/js/chat.js"></script>
    <script type="text/javascript"> 
      $(document).ready(function(){
        $('#ret_url').val("index.php");

        var originColor = '#E3E3E3';
        $('#newspad #title').click(function(){
            $('#newspad #mainarea').slideToggle();
            $('#title i').toggleClass('icon-double-angle-down');
            $('#title i').toggleClass('icon-double-angle-up');
            /* change color, unnecessary in this theme
            var tmp = $('#newspad').css('background-color');
            $('#newspad').css('background-color', originColor);
            originColor = tmp;
             */
        });
      });
    </script>

    <script type="text/javascript">
$(function () {
    var chart;
    $(document).ready(function() {
        chart = new Highcharts.Chart({
            chart: {
                renderTo: 'chart',
                type: 'area'
            },
            title: {
                text: 'The past 6 years'
            },
            credits: {
              enabled: false
            },
            xAxis: {
              type: 'datetime'
            },
            yAxis: {
                title: {
                    text: 'Number'
                },
                labels: {
                    formatter: function() {
                        return this.value;
                    }
                }
            },
            tooltip: {
                formatter: function() {
                  var d=new Date();
                  d.setTime(this.x);
                    return d.getFullYear()+'-'+(d.getMonth()+1) + ': <br>' +this.y;
                }
            },
            plotOptions: {
                area: {
                    // dataLabels: {
                    //     enabled: true
                    // },
                    enableMouseTracking: true,
                    // pointStart: 200601,
                    marker: {
                        enabled: true,
                        symbol: 'circle',
                        radius: 4,
                        states: { hover: {
                                enabled: true
                            }
                        }
                    }
                }
            },
            series: [{
                name: 'Submit',
                data: [[Date.UTC(2006,0,1),3],[Date.UTC(2006,1,1),897],[Date.UTC(2006,2,1),1015],[Date.UTC(2006,3,1),416],[Date.UTC(2006,4,1),312],[Date.UTC(2006,5,1),112],[Date.UTC(2006,6,1),3086],[Date.UTC(2006,7,1),2134],[Date.UTC(2006,8,1),771],[Date.UTC(2006,9,1),2130],[Date.UTC(2006,10,1),3155],[Date.UTC(2006,11,1),3990],[Date.UTC(2007,0,1),2088],[Date.UTC(2007,1,1),4590],[Date.UTC(2007,2,1),2700],[Date.UTC(2007,3,1),3874],[Date.UTC(2007,4,1),3423],[Date.UTC(2007,5,1),4527],[Date.UTC(2007,6,1),9316],[Date.UTC(2007,7,1),10607],[Date.UTC(2007,8,1),7163],[Date.UTC(2007,9,1),14922],[Date.UTC(2007,10,1),6872],[Date.UTC(2007,11,1),1783],[Date.UTC(2008,0,1),6450],[Date.UTC(2008,1,1),2891],[Date.UTC(2008,2,1),6690],[Date.UTC(2008,3,1),3974],[Date.UTC(2008,4,1),3264],[Date.UTC(2008,5,1),1153],[Date.UTC(2008,6,1),12391],[Date.UTC(2008,7,1),2372],[Date.UTC(2008,8,1),1510],[Date.UTC(2008,9,1),2032],[Date.UTC(2008,10,1),4680],[Date.UTC(2008,11,1),1243],[Date.UTC(2009,0,1),3023],[Date.UTC(2009,1,1),3953],[Date.UTC(2009,2,1),3015],[Date.UTC(2009,3,1),3738],[Date.UTC(2009,4,1),3770],[Date.UTC(2009,5,1),3550],[Date.UTC(2009,6,1),6084],[Date.UTC(2009,7,1),7438],[Date.UTC(2009,8,1),3449],[Date.UTC(2009,9,1),5766],[Date.UTC(2009,10,1),5298],[Date.UTC(2009,11,1),1847],[Date.UTC(2010,0,1),1740],[Date.UTC(2010,1,1),2884],[Date.UTC(2010,2,1),1372],[Date.UTC(2010,3,1),2386],[Date.UTC(2010,4,1),2410],[Date.UTC(2010,5,1),2092],[Date.UTC(2010,6,1),8868],[Date.UTC(2010,7,1),4465],[Date.UTC(2010,8,1),5410],[Date.UTC(2010,9,1),6094],[Date.UTC(2010,10,1),4744],[Date.UTC(2010,11,1),3301],[Date.UTC(2011,0,1),4374],[Date.UTC(2011,1,1),3254],[Date.UTC(2011,2,1),4368],[Date.UTC(2011,3,1),4390],[Date.UTC(2011,4,1),3388],[Date.UTC(2011,5,1),1393],[Date.UTC(2011,6,1),7256],[Date.UTC(2011,7,1),5983],[Date.UTC(2011,8,1),3391],[Date.UTC(2011,9,1),4335],[Date.UTC(2011,10,1),4778],[Date.UTC(2011,11,1),1782],[Date.UTC(2012,0,1),3894],[Date.UTC(2012,1,1),3594],[Date.UTC(2012,2,1),3508],[Date.UTC(2012,3,1),3940],[Date.UTC(2012,4,1),4362],[Date.UTC(2012,5,1),3897],[Date.UTC(2012,6,1),9430],[Date.UTC(2012,7,1),8552],[Date.UTC(2012,8,1),8441],[Date.UTC(2012,9,1),8613],[Date.UTC(2012,10,1),5174],[Date.UTC(2012,11,1),8684] ]
            },{
                name: 'Accepted',
                data: [[Date.UTC(2006,0,1),1],[Date.UTC(2006,1,1),372],[Date.UTC(2006,2,1),366],[Date.UTC(2006,3,1),161],[Date.UTC(2006,4,1),115],[Date.UTC(2006,5,1),31],[Date.UTC(2006,6,1),1130],[Date.UTC(2006,7,1),785],[Date.UTC(2006,8,1),323],[Date.UTC(2006,9,1),923],[Date.UTC(2006,10,1),1274],[Date.UTC(2006,11,1),1700],[Date.UTC(2007,0,1),920],[Date.UTC(2007,1,1),1953],[Date.UTC(2007,2,1),1092],[Date.UTC(2007,3,1),1533],[Date.UTC(2007,4,1),1322],[Date.UTC(2007,5,1),1839],[Date.UTC(2007,6,1),4239],[Date.UTC(2007,7,1),4489],[Date.UTC(2007,8,1),3042],[Date.UTC(2007,9,1),6553],[Date.UTC(2007,10,1),3366],[Date.UTC(2007,11,1),793],[Date.UTC(2008,0,1),3286],[Date.UTC(2008,1,1),1371],[Date.UTC(2008,2,1),2906],[Date.UTC(2008,3,1),1643],[Date.UTC(2008,4,1),1278],[Date.UTC(2008,5,1),603],[Date.UTC(2008,6,1),5997],[Date.UTC(2008,7,1),977],[Date.UTC(2008,8,1),624],[Date.UTC(2008,9,1),933],[Date.UTC(2008,10,1),2514],[Date.UTC(2008,11,1),644],[Date.UTC(2009,0,1),1321],[Date.UTC(2009,1,1),1800],[Date.UTC(2009,2,1),1329],[Date.UTC(2009,3,1),1626],[Date.UTC(2009,4,1),1671],[Date.UTC(2009,5,1),1448],[Date.UTC(2009,6,1),2762],[Date.UTC(2009,7,1),3429],[Date.UTC(2009,8,1),1397],[Date.UTC(2009,9,1),2572],[Date.UTC(2009,10,1),2724],[Date.UTC(2009,11,1),1001],[Date.UTC(2010,0,1),858],[Date.UTC(2010,1,1),1488],[Date.UTC(2010,2,1),651],[Date.UTC(2010,3,1),1165],[Date.UTC(2010,4,1),1132],[Date.UTC(2010,5,1),1047],[Date.UTC(2010,6,1),3823],[Date.UTC(2010,7,1),1994],[Date.UTC(2010,8,1),2325],[Date.UTC(2010,9,1),2618],[Date.UTC(2010,10,1),2058],[Date.UTC(2010,11,1),1432],[Date.UTC(2011,0,1),1735],[Date.UTC(2011,1,1),1126],[Date.UTC(2011,2,1),1847],[Date.UTC(2011,3,1),1954],[Date.UTC(2011,4,1),1310],[Date.UTC(2011,5,1),426],[Date.UTC(2011,6,1),3240],[Date.UTC(2011,7,1),3046],[Date.UTC(2011,8,1),1755],[Date.UTC(2011,9,1),2030],[Date.UTC(2011,10,1),2522],[Date.UTC(2011,11,1),983],[Date.UTC(2012,0,1),1966],[Date.UTC(2012,1,1),1754],[Date.UTC(2012,2,1),1784],[Date.UTC(2012,3,1),1780],[Date.UTC(2012,4,1),1962],[Date.UTC(2012,5,1),1615],[Date.UTC(2012,6,1),4512],[Date.UTC(2012,7,1),4341],[Date.UTC(2012,8,1),4405],[Date.UTC(2012,9,1),4163],[Date.UTC(2012,10,1),2458],[Date.UTC(2012,11,1),3700] ]
            }]
        });
    });
    
});
    </script>

  </body>
</html>
