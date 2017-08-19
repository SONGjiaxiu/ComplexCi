mkdir -p regressionResult

ballRadius=0

if (( $# >= 1 ))
then
    ballRadius=$1
fi

echoWhetherPassed()
{
	while read compareString1 <&3 && read compareString2 <&4; do     
		 
		 printf "\n\n"
		 
		 if [ "$compareString1" == "$compareString2" ] >/dev/null ; then
			 echo "$compareString1" "$compareString2"  Passed
		 else
			 echo "$compareString1" "$compareString2" Not Passed
		 fi 
		 
		 printf "\n\n"
		 
	done 3</tmp/overrallScore 4<regressionTest${ballRadius}
	
}

serIDRegression="regressionTest"_$(date "+%y_%m_%d_%H_%M_%S")_"$ballRadius"
regressionResult=`readlink -f regressionResult/$serIDRegression.csv`
echo $regressionResult

for((i=0;i<19;i++))
do
	echo Now $i
	date
	./executeAll.sh $ballRadius 1 500 $i 0.001
	./calGroovyBenchmark.sh $regressionResult
	
	echoWhetherPassed
	
done


for i in 5 6 7 8
do
	echo Now $i
	date
	./executeAll.sh $ballRadius 1 500 $i 0.0
	./calGroovyBenchmark.sh $regressionResult
	
	echoWhetherPassed
	
done




