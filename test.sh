#!/bin/bash
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
APP='./bin/bin_ctl';
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# run app
function run_app()
{
	local RESULT=0;
	local STDOUT;

	if [ "${FLAG_VALGRIND}" != "1" ];
	then
		STDOUT=$("${APP}" "${@}");
		RESULT="${?}";
	else
		local VAL="valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-reachable=yes --log-file=valgrind.log ${APP} ${@}";
		STDOUT=$(${VAL});
		RESULT="${?}";

		echo '--------------------------' >> valgrind.all.log;
		cat valgrind.log >> valgrind.all.log;
		rm -rf valgrind.log;
	fi


	if [ "${STDOUT}" != "" ];
	then
		echo "${STDOUT}";
	fi

	return "${RESULT}";
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test1
function test1()
{
	local TMP;
	TMP="$(mktemp)";
	if [ "${?}" != "0" ];
	then
		echo "can't make tmp file";
		exit 1;
	fi


	cat /dev/zero | head -c 13 > "${TMP}";

# H  e  l  l  o     w  o  r  l  d  ! \n
#48 65 6C 6C 6F 20 77 6F 72 6C 64 21 0A

	run_app -u8 -offset  0 -set  72 "${TMP}" < /dev/null; # 0x48 72
	run_app -u8 -offset  1 -set 101 "${TMP}" < /dev/null; # 0x65 101
	run_app -u8 -offset  2 -set 108 "${TMP}" < /dev/null; # 0x6C 108
	run_app -u8 -offset  3 -set 108 "${TMP}" < /dev/null; # 0x6C 108
	run_app -u8 -offset  4 -set 111 "${TMP}" < /dev/null; # 0x6F 111
	run_app -u8 -offset  5 -set  32 "${TMP}" < /dev/null; # 0x20 32
	run_app -u8 -offset  6 -set 119 "${TMP}" < /dev/null; # 0x77 119
	run_app -u8 -offset  7 -set 111 "${TMP}" < /dev/null; # 0x6F 111
	run_app -u8 -offset  8 -set 114 "${TMP}" < /dev/null; # 0x72 114
	run_app -u8 -offset  9 -set 108 "${TMP}" < /dev/null; # 0x6C 108
	run_app -u8 -offset 10 -set 100 "${TMP}" < /dev/null; # 0x64 100
	run_app -u8 -offset 11 -set  33 "${TMP}" < /dev/null; # 0x21 33
	run_app -u8 -offset 12 -set  10 "${TMP}" < /dev/null; # 0x0A 10


	local X=$(cat "${TMP}");

	if [ "${X}" != "Hello world!" ];
	then
		echo "ERROR: result different...";
		echo -n "X: ${X}";
		exit 1;
	fi


	local A=$(run_app -u8 -offset  0 "${TMP}" < /dev/null);

	if [ "${A}" != "0x48" ];
	then
		echo "ERROR: result different...";
		echo -n "A: ${A}";
		exit 1;
	fi


	rm "${TMP}";
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test2
function test2()
{
	local TMP1;
	TMP1="$(mktemp)";
	if [ "${?}" != "0" ];
	then
		echo "can't make tmp file";
		exit 1;
	fi

	local TMP2;
	TMP2="$(mktemp)";
	if [ "${?}" != "0" ];
	then
		echo "can't make tmp file";
		exit 1;
	fi

	echo "Hello world!" > "${TMP1}";

	local SIZE=$(stat --format '%s' "${TMP1}");

	cat /dev/zero | head -c "${SIZE}" > "${TMP2}";


	for ((i=0; i < SIZE; i++));
	do

		local VAL=$(run_app -u8 -offset ${i} "${TMP1}" < /dev/null);

		if [ "${FLAG_DEBUG}" != "" ];
		then
			echo "VAL:${VAL}";
		fi

		run_app -u8 -offset ${i} -set ${VAL} "${TMP2}" < /dev/null &> /dev/null;

	done


	local X=$(cat "${TMP2}");

	if [ "${X}" != "Hello world!" ];
	then
		echo "ERROR: result different...";
		echo -n "X: ${X}";
		exit 1;
	fi


	rm "${TMP1}";
	rm "${TMP2}";
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# test3
function test3()
{
	local TMP1;
	TMP1="$(mktemp)";
	if [ "${?}" != "0" ];
	then
		echo "can't make tmp file";
		exit 1;
	fi


	cat /dev/zero | head -c 65600 > "${TMP1}";

	local A='0xed';

	run_app -u8 -offset 65534 -set "${A}" "${TMP1}" < /dev/null &> /dev/null;

	local B=$(run_app -u8 -offset 0xfffe "${TMP1}" < /dev/null);

	if [ "${A}" != "${B}" ];
	then
		echo "ERROR: result different...";
		echo "A: ${A}";
		echo "B: ${B}";
		exit 1;
	fi


	rm "${TMP1}";
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# check depends
function check_prog()
{
	for i in ${1};
	do
		if [ "$(which ${i})" == "" ];
		then
			echo "FATAL: you must install \"${i}\"...";
			return 1;
		fi
	done

	return 0;
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
# general function
function main()
{
	if [ ! -e "${APP}" ];
	then
		echo "ERROR: make it";
		return 1;
	fi


	check_prog "awk cat echo md5sum mktemp rm stat";
	if [ "${?}" != "0" ];
	then
		return 1;
	fi


	test1;
	test2;
	test3;


	echo "ok, test passed";
	return 0;
}
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
main "${@}";

exit "${?}";
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
