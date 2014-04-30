fenstr=$1
x0=0
y0=0
x1=0
y1=0

if [ $# == "5" ]; then
y0=$2
x0=$3
y1=$4
x1=$5
fi

i=0
x=0
y=1
while [ $i -lt ${#fenstr} ]
do
	c=${fenstr:i:1}
	if [ $c == r ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;30m车[0m "; else echo -n -e "[1;30m车[0m ";fi
	elif [ $c == n ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;30m马[0m "; else echo -n -e "[1;30m马[0m ";fi
	elif [ $c == b ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;30m象[0m "; else echo -n -e "[1;30m象[0m ";fi
	elif [ $c == a ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;30m士[0m "; else echo -n -e "[1;30m士[0m ";fi
	elif [ $c == k ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;30m将[0m "; else echo -n -e "[1;30m将[0m ";fi
	elif [ $c == c ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;30m炮[0m "; else echo -n -e "[1;30m炮[0m ";fi
	elif [ $c == p ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;30m卒[0m "; else echo -n -e "[1;30m卒[0m ";fi
	elif [ $c == R ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;31m车[0m "; else echo -n -e "[1;31m车[0m ";fi
	elif [ $c == N ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;31m马[0m "; else echo -n -e "[1;31m马[0m ";fi
	elif [ $c == B ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;31m相[0m "; else echo -n -e "[1;31m相[0m ";fi
	elif [ $c == A ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;31m仕[0m "; else echo -n -e "[1;31m仕[0m ";fi
	elif [ $c == K ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;31m帅[0m "; else echo -n -e "[1;31m帅[0m ";fi
	elif [ $c == C ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;31m炮[0m "; else echo -n -e "[1;31m炮[0m ";fi
	elif [ $c == P ]; then x=$[x+1]; if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then echo -n -e "[42;31m兵[0m "; else echo -n -e "[1;31m兵[0m ";fi
	elif [ $c == '/' ]; then echo ""; x=0; y=$[y+1];
	else
		j=1
		while [ $j -le $c ]
		do
			x=$[x+1];
			if [[ $x == $x0 && $y == $y0 || $x == $x1 && $y == $y1 ]]; then
				echo -n -e "[42;31m  [0m "
			else
				echo -n -e "   "
			fi
			j=$[j+1]
		done
	fi
	i=$[i+1]
done
echo ""
