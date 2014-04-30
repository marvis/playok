win_id=114989
i=1
while [ 1 ];
do
	screencapture -o -l$win_id screen$i.png; i=$[i+1];
	sleep 1
done
