for f in $1/f*
do
  id=$(basename $f)
  echo $id "Ecol/$id" 
done > $2


