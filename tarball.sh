DIR='ep1-albert-renan'
rm -rf $DIR
mkdir $DIR
cp -av * $DIR
rmdir $DIR/$DIR
tar -pczf ep1-albert-renan.tar.gz $DIR
rm -rf $DIR
