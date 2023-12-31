# TelematicsPilot-Microprocessor-Rpi

#GPS Data Format 

		 Found GNGGA string.  It has 14 commas total.  Its NMEA sentence structure is:

		 $GPGAA,hhmmss.ss,ddmm.mmmm,n,dddmm.mmmm,e,q,ss,y.y,a.a,z,g.g,z,t.t,iii*CC
		 |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
		 0   	   1         2         3         4         5         6         7
		 0123456789012345678901234567890123456789012345678901234567890123456789012

		 where:

		 GPGAA		: GPS fixed data identifier
		 hhmmss.ss	: Coordinated Universal Time (UTC), also known as GMT
		 ddmm.mmmm,n	: Latitude in degrees, minutes and cardinal sign
		 dddmm.mmmm,e	: Longitude in degrees, minutes and cardinal sign
		 q		: Quality of fix.  1 = there is a fix
		 ss		: Number of satellites being used
		 y.y		: Horizontal dilution of precision
		 a.a,M		: GPS antenna altitude in meters
		 g.g,M		: geoidal separation in meters
		 t.t		: Age of the defferential correction data
		 iiii		: Deferential station's ID
		 *CC		: checksum for the sentence

         $GPGSA GPS DOP and active satellites

		 $GPGSA,A,3,17,02,30,04,05,10,09,06,31,12,,,1.2,0.8,0.9*2B
		 |      | |                                  |   |   |  | 
		 0      2 2        3-14                      15  16  17 18 

		 where
		1   = Mode:
			M=Manual, forced to operate in 2D or 3D
			A=Automatic, 3D/2D
		2   = Mode:
			1=Fix not available
			2=2D
			3=3D
		3-14 = IDs of SVs used in position fix (null for unused fields)
		15   = PDOP
		16   = HDOP
		17   = VDOP
		18   = Check sum

# CPPUTEST setup steps.
   step 1:
   To install cpputest on windows follow : https://github.com/miguelmoraperea/guide_setup_cpputest_eclipse_win_7 till 13th step.

   step 2: copy the cpputest project to your current project or make changes to makefile for variable CPPUTEST_HOME

   			The CppUTest repository provides a file called “MakefileWorker.mk”. It provides a lot of functionality which makes building with CppUTest simple. The file lives under the “build” directory in the git repository. For this tutorial we’re going to assume it’s been copied to the ‘AllTests/’ directory.

   step 3:  Inside cygwin console
			sopujari@USBLRSOPUJARI2 /cygdrive/c/Users/sopujari/gitcode/telematics/Telematics-Rpi/tests
			$ ls
				alltests.cpp  cpputest  lib  makefile  objs

			sopujari@USBLRSOPUJARI2 /cygdrive/c/Users/sopujari/gitcode/telematics/Telematics-Rpi/tests
			$ cd cpputest/

			sopujari@USBLRSOPUJARI2 /cygdrive/c/Users/sopujari/gitcode/telematics/Telematics-Rpi/tests/cpputest
			$ ls
				AUTHORS         CppUTest.mak                     Doxyfile                       README                           autogen.sh      config.h.in             depcomp     ltmain.sh       platforms
				CMakeLists.txt  CppUTest.sln                     INSTALL                        README.md                        build           config.sub              docs        m4              platforms_examples
				COPYING         CppUTest.vcproj                  Makefile.am                    README_CppUTest_for_C.txt        builds          configure               examples    makeAndRun.bat  scripts
				ChangeLog       CppUTest.vcxproj                 Makefile.in                    README_InstallCppUTest.txt       cmake           configure.ac            generated   makeVS2008.bat  src
				CppUTest.dep    CppUTestConfig.cmake.build.in    Makefile_CppUTestExt           README_UsersOfPriorVersions.txt  compile         cpputest.pc.in          include     makeVS201x.bat  test-driver
				CppUTest.dsp    CppUTestConfig.cmake.install.in  Makefile_using_MakefileWorker  aclocal.m4                       config.guess    cpputest_build          install-sh  makeVc6.bat     tests
				CppUTest.dsw    CppUTest_VS201x.sln              NEWS                           appveyor.yml                     config.h.cmake  cpputest_doxy_gen.conf  lib         missing         valgrind.suppressions

			sopujari@USBLRSOPUJARI2 /cygdrive/c/Users/sopujari/gitcode/telematics/Telematics-Rpi/tests/cpputest
			$ autoreconf -fi

			sopujari@USBLRSOPUJARI2 /cygdrive/c/Users/sopujari/gitcode/telematics/Telematics-Rpi/tests/cpputest
			$ ./configure

			sopujari@USBLRSOPUJARI2 /cygdrive/c/Users/sopujari/gitcode/telematics/Telematics-Rpi/tests/cpputest
			$ make tdd

			sopujari@USBLRSOPUJARI2 /cygdrive/c/Users/sopujari/gitcode/telematics/Telematics-Rpi/tests/cpputest
			$ cd ..

			sopujari@USBLRSOPUJARI2 /cygdrive/c/Users/sopujari/gitcode/telematics/Telematics-Rpi/tests
			$ make
				compiling alltests.cpp
				Linking CustomTests_tests
				Running CustomTests_tests
				.
				OK (1 tests, 1 ran, 0 checks, 0 ignored, 0 filtered out, 1 ms)

# To Enable logging for a specific module or to write the logs for a file :
    GPS_LOG_MODULE_ID 2, SERIAL_LOG_MODULE_ID 3, CAN_LOG_MODULE_ID 4, CC_LOG_MODULE_ID 5, CLOUD_LOG_MODULE_ID 6
    sudo ./Telematic -m 3 ==> This will enable the logs only for Serial_interface module
    sudo ./Telematic -m 2 -f 1 ==> This will enable the logs only for GPS module and -f flag will enable writing to a file

# VIRTUAL CAN and virtual server can socket setup steps for simulating OBD2 data on request-response approach:

    # Step 1: Install can-utils from the below Linux command 
            $ sudo apt-get update
            $ sudo apt-get install can-utils

    # Step 2: Run the below commands one by one to create a virtual can 
            $ sudo modprobe vcan &&
            $ sudo ip link add dev vcan0 type vcan &&
            $ sudo ip link set up vcan0
    
    # Step 3: Run the below command to compile virtual_can_server.c file in a different terminal 
              to run a virtual can socket in the background to simulate OBD2 data, which acts as a can server
            $ gcc Telematics-Rpi/src/can_bus/virtual_can_server.c -o virtualcan
            $ ./virtualcan

    # Step 4: Virtual CanSocket is ready, 
              the telematic application can request to this virtual can to fetch the PID data by sending request can frame
        
    # Step 5: (Option)
              Add the below commands in /etc/rc.local to Run Virtual CAN Socket background automatically when os boots

			    sudo modprobe vcan &&
				sudo ip link add dev vcan0 type vcan &&
				sudo ip link set up vcan0 &&
				cd /home/pi/Telematics-Rpi/src/can_bus &&
				gcc virtual_can_server.c -o virtualcan &&
				./virtualcan &

#Telematics DB schema
|ID | creation_time | Latitude| LatSign | Longitude|LongSign |	PDOP |	HDOP |	VDOP |	Serial |	VIN	Speed |  supported_Pids |Dist_Travelled |	Idle_time |	Veh_in_Service |	Motion |	Voltage |	PTO |	AccX |	AccY |	AccZ |	RPM |	Temperature |
|---|---------------|---------|---------|----------|---------|--------|-------|-------|---------|--------------|----------------|--------------|----------------|-----------|------------|-------|--------|-------|-------|------|---------------|--------------|

# Code Coverage

	Unit testing would not be complete without a coverage report. The go-to tool for this for projects using gcc is gcov, available as part of the standard suite of gcc utilities. Cpputest integrates easily with gcov, all you need to do is add this line to the makefile:

	CPPUTEST_USE_GCOV=Y
	Next, we need to make sure that the filterGcov.sh script from this repo is in ‘/scripts/filterGcov.sh’ relative to wherever you set ‘CPPUTEST_HOME’ to be. It also needs to have execute perms.

	In the example Makefile, it would be deployed to ‘/usr/local/scripts/filterGcov.sh’. If you’re running CppUTest from a repo checkout, everything should work without modification.

	With that in place, you can simply run ‘make gcov’ and the analysis will be generated for you. In our case we’ll need to ‘make -B’ to rebuild the object files with gcov enabled:

	[]$ make -B gcov
	< compilation output >
	for d in /Users/ykuperman/code/blogpost/qa/src/code ; do \
					FILES=`ls $d/*.c $d/*.cc $d/*.cpp 2> /dev/null` ; \
					gcov  --object-directory objs/$d $FILES >> gcov_output.txt 2>>gcov_error.txt ; \
			done
	for f in  ; do \
					gcov  --object-directory objs/$f $f >> gcov_output.txt 2>>gcov_error.txt ; \
			done
	/usr/local/scripts/filterGcov.sh gcov_output.txt gcov_error.txt gcov_report.txt example.txt
	cat gcov_report.txt
	100.00%   /Users/ykuperman/code/blogpost/qa/src/code/code.cpp
	mkdir -p gcov
	mv *.gcov gcov
	mv gcov_* gcov
	See gcov directory for details
	This will output a number of files to a new ‘gcov’ directory. These are:

	code.cpp.gcov – the actual ‘gcov’ file for the code being tested
	gcov_error.txt – an error report (in our case, it should be empty)
	gcov_output.txt – the actual output of the gcov command that was run
	gcov_report.txt – a summary of the coverage for each file under test
	gcov_report.txt.html – an html version of gcov_report
	
	link : https://www.sparkpost.com/blog/getting-started-cpputest/

	Current Code Coverage:
	55.51%   ../src/logger/logger.c
	57.35%   ../src/serial_interface/serial_config.c
	68.46%   ../src/can_bus/can_interface.c
	71.04%   ../src/gps_module/gps_read.c
	76.58%   ../src/can_bus/can_bus.c
	93.33%   ../src/utils/common_utils.c
	96.55%   ../src/client_controller/client_controller.c

	# BLUETOOTH CAN SETUP 

	Install the bluetooth package using below command
	sudo apt-get install libbluetooth3-dev

	Run the telematics application using following command 

	sudo ./telematic -c 8 

	To use virtual can run  

	sudo ./telematic

	# To Run OTA JOBS

	Modify the telematics_update.sh as per your Thing names and endpoints as shown below

	sudo ./jobs_demo_mosquitto -o -n THING-NAME -h aws-endpoint --certfile .Aws-certificate.pem.crt --keyfile ./path/to/Aws-private.pem.key --destination ./​

	Where,​
​
		-o : run once, exit after the first job is finished.​

		-n : thing name​

		-h : mqtt host to connect to.​

		-p : network port to connect to. Defaults to 8883.​
​
		--certfile : AWS certificate .pem.crt file.​
		
		--keyfile : AWS private.pem.key file.​

		--destination : download file path	

	Execute the telematics_update script 

	./telematics_update.sh
