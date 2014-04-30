win_id=`./GetWindowID Safari Xiangqi`
i=1
while [ 1 ];
do
	screencapture -o -l$win_id screen$i.png; 
	#node ./xqwizard_interface/xqcli.js -f `./main screen$i.png`
	./main screen$i.png -v
	i=$[i+1];

	sleep 1
done
