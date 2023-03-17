
echo -e "--------------------------------------\n"
echo -e "segfault.jpg\n"
djpeg /comp/40/bin/images/segfault.jpg > test1.ppm
djpeg /comp/40/bin/images/segfault.jpg | ./40image -c > test.compress
./40image -d test.compress > test.ppm
./ppmdiff test.ppm test1.ppm

echo -e "--------------------------------------\n"
echo -e "halligan.jpg\n"
djpeg /comp/40/bin/images/halligan.jpg > test1.ppm
djpeg /comp/40/bin/images/halligan.jpg | ./40image -c > test.compress
./40image -d test.compress > test.ppm
./ppmdiff test.ppm test1.ppm

echo -e "--------------------------------------\n"
echo -e "rock.jpg\n"
djpeg /comp/40/bin/images/rock.jpg > test1.ppm
djpeg /comp/40/bin/images/rock.jpg | ./40image -c > test.compress
./40image -d test.compress > test.ppm
./ppmdiff test.ppm test1.ppm

echo -e "--------------------------------------\n"
echo -e "animals.jpg\n"
djpeg /comp/40/bin/images/animals.jpg > test1.ppm
djpeg /comp/40/bin/images/animals.jpg | ./40image -c > test.compress
./40image -d test.compress > test.ppm
./ppmdiff test.ppm test1.ppm

echo -e "--------------------------------------\n"
echo -e "wind-cave.jpg\n"
djpeg /comp/40/bin/images/wind-cave.jpg > test1.ppm
djpeg /comp/40/bin/images/wind-cave.jpg | ./40image -c > test.compress
./40image -d test.compress > test.ppm
./ppmdiff test.ppm test1.ppm

echo -e "--------------------------------------\n"
echo -e "desert.jpg\n"
djpeg /comp/40/bin/images/desert.jpg > test1.ppm
djpeg /comp/40/bin/images/desert.jpg | ./40image -c > test.compress
./40image -d test.compress > test.ppm
./ppmdiff test.ppm test1.ppm

echo -e "--------------------------------------\n"
echo -e "from-wind-cave.jpg\n"
djpeg /comp/40/bin/images/from-wind-cave.jpg > test1.ppm
djpeg /comp/40/bin/images/from-wind-cave.jpg | ./40image -c > test.compress
./40image -d test.compress > test.ppm
./ppmdiff test.ppm test1.ppm
