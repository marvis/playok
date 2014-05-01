# step1: check the start fenstr
sleep 2
result=""
while [ "$result" == "" ]; 
do
	echo "find window"
	result=`./GetWindowID Safari Xiangqi`
	sleep 1
done

win_id=`echo $result | awk '{print $1}'`
win_x0=`echo $result | awk '{print $2}'`
win_y0=`echo $result | awk '{print $3}'`

screencapture -o -l$win_id screen.png
result=`main screen.png`
fenstr=`echo $result | awk '{print $1}'`
mycolor=`echo $result | awk '{print $2}'`
whonext=`echo $result | awk '{print $3}'`
if [ $mycolor == $whonext ]; then
	pre_fenstr=""
else
	pre_fenstr=$fenstr
fi

failcount=0
#echo "=== start move ==="

#step2: start play
while [ 1 ]
do
	screencapture -o -l$win_id screen.png

	result=`./main screen.png`
	fenstr=`echo $result | awk '{print $1}'`
	if [ "$fenstr" == "" ]; then 
		echo "无法识别"
		sleep 0.2
		failcount=$[failcount+1]
		if [ $failcount -ge 4 ]; then break; fi
		continue
	fi
	ch_x0=`echo $result | awk '{print $4}'`
	ch_y0=`echo $result | awk '{print $5}'`
	xstep=`echo $result | awk '{print $6}'`
	ystep=`echo $result | awk '{print $7}'`
	if [ "$fenstr" == "$prev_fenstr" ]; then
		#echo "对方没有动作"
		sleep 0.2
		continue
	fi

	isvalid1=`./isfenvalid $fenstr`
	if [ "$isvalid1" == "false" ]; then 
		echo "不合法的棋谱"
		sleep 0.2
		failcount=$[failcount+1]
		if [ $failcount -ge 4 ]; then break; fi
		continue; 
	fi

	isvalid2=`./isfenvalid $prev_fenstr $fenstr`
	if [ "$isvalid2" == "false" ]; then
		echo "不合法的走法"
		sleep 0.2
		failcount=$[failcount+1]
		if [ $failcount -ge 4 ]; then break; fi
		continue;
	fi
	failcount=0

	#clear
	echo "-------------------------------"
	./fenstr2matrix.sh $fenstr `./whichmoves $prev_fenstr $fenstr`
	echo "-------------------------------"

	nextmove=`node xqwizard_interface/xqcli.js -f "$fenstr $mycolor"`
	#echo $nextmove

	x0=`echo "$nextmove" | awk '{print $1}'`
	y0=`echo "$nextmove" | awk '{print $2}'`
	x1=`echo "$nextmove" | awk '{print $3}'`
	y1=`echo "$nextmove" | awk '{print $4}'`
	status=`echo "$nextmove" | awk '{print $5}'`
    prev_fenstr=`echo "$nextmove" | awk '{print $6}'`
	x0=`echo "($win_x0+$ch_x0+$x0*$xstep)*10/10"|bc`
	y0=`echo "($win_y0+$ch_y0+$y0*$ystep)*10/10"|bc`
	x1=`echo "($win_x0+$ch_x0+$x1*$xstep)*10/10"|bc`
	y1=`echo "($win_y0+$ch_y0+$y1*$ystep)*10/10"|bc`
	#echo "start move"
	cliclick c:$x0,$y0
	sleep 0.1
	cliclick c:$x1,$y1

	if [ "$status" == "1" ]; then
		echo "game over"
		break
	fi

	#clear
	echo "-------------------------------"
	./fenstr2matrix.sh $prev_fenstr `./whichmoves $fenstr $prev_fenstr`
	echo "-------------------------------"

	sleep 0.2 
done
