
for f in $1/f*
do
  id=$(basename $f)
  echo $id $f 
done > $2


