
gcc -g -Wall -DIMGW_BMP_WRITE_MAIN -I../ ../imgw_bmp/src/imgw_bmp_write.c -o imgw_bmp_write_test.out
gcc -g -Wall -DIMGW_BMP_READ_MAIN -I../ ../imgw_bmp/src/imgw_bmp_read.c ../imgw_bmp/src/imgw_bmp_write.c -o imgw_bmp_read_test.out

./imgw_bmp_write_test.out
echo "imgw_bmp_write_test = $?"

./imgw_bmp_read_test.out
echo "imgw_bmp_read_test = $?"
