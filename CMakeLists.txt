project(shell_cmds)

cmake_minimum_required(VERSION 2.4.0)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include(darling_exe)
add_definitions(-DTARGET_OS_MAC=1)
add_definitions(-D__APPLE__ -D__DYNAMIC__)
# add_definitions(-DSUPPORT_UTMPX)

add_definitions(
	-Wno-incompatible-pointer-types
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__DARWIN_UNIX03 -fPIC -ggdb -O0")

add_definitions(-nostdinc)

include_directories(${CMAKE_SOURCE_DIR}/src/libc/include/FreeBSD)
include_directories(${CMAKE_SOURCE_DIR}/src/libc/include/NetBSD)
include_directories(${CMAKE_SOURCE_DIR}/src/libc/gen)
include_directories(${CMAKE_SOURCE_DIR}/src/libc/darwin)
include_directories(${CMAKE_SOURCE_DIR}/src/libc/stdtime/FreeBSD)
include_directories(${CMAKE_SOURCE_DIR}/src/launchd/liblaunch)
include_directories(${CMAKE_SOURCE_DIR}/src/external/libdispatch)
include_directories(${CMAKE_SOURCE_DIR}/src/kernel/libsyscall/wrappers/libproc)

BISON_TARGET(DateParser find/getdate.y ${CMAKE_CURRENT_BINARY_DIR}/find_getdate.c)
BISON_TARGET(Expr expr/expr.y ${CMAKE_CURRENT_BINARY_DIR}/expr.c)

include(InstallSymlink)
set(BINARY_PACKAGING_MODE ON)

add_darling_executable(basename basename/basename.c)
add_darling_executable(date date/date.c date/netdate.c date/vary.c)
add_darling_executable(dirname dirname/dirname.c)
add_darling_executable(echo echo/echo.c)
add_darling_executable(pwd pwd/pwd.c)
add_darling_executable(env env/env.c env/envopts.c)
add_darling_executable(yes yes/yes.c)
add_darling_executable(false false/false.c)
add_darling_executable(true true/true.c)
add_darling_executable(apply apply/apply.c)
add_darling_executable(getopt getopt/getopt.c)
add_darling_executable(hostname hostname/hostname.c)
add_darling_executable(kill kill/kill.c)
add_darling_executable(killall killall/killall.c)
add_darling_executable(mktemp mktemp/mktemp.c)
add_darling_executable(nice nice/nice.c)
add_darling_executable(nohup nohup/nohup.c)
add_darling_executable(printenv printenv/printenv.c)
add_darling_executable(printf printf/printf.c)
add_darling_executable(renice renice/renice.c)
add_darling_executable(script script/script.c)
add_darling_executable(seq seq/seq.c)
add_darling_executable(shlock shlock/shlock.c)
add_darling_executable(sleep sleep/sleep.c)
add_darling_executable(systime systime/systime.c)
add_darling_executable(tee tee/tee.c)
#add_darling_executable(test-bin test/test.c)
#set_target_properties(test-bin PROPERTIES OUTPUT_NAME test)
add_darling_executable(time time/time.c)
add_darling_executable(uname uname/uname.c)
add_darling_executable(users users/users.c)
add_darling_executable(what what/what.c)
add_darling_executable(whereis whereis/whereis.c)
add_darling_executable(which which/which.c)
#add_darling_executable(who who/who.c who/utmpentry.c)
add_darling_executable(xargs xargs/strnsubst.c xargs/xargs.c)
add_darling_executable(find find/find.c  find/function.c  find/ls.c  find/main.c  find/misc.c  find/operator.c  find/option.c ${CMAKE_CURRENT_BINARY_DIR}/find_getdate.c)
add_darling_executable(hexdump hexdump/conv.c  hexdump/display.c  hexdump/hexdump.c  hexdump/hexsyntax.c  hexdump/odsyntax.c  hexdump/parse.c)
add_darling_executable(jot jot/jot.c)
add_darling_executable(id id/id.c)
add_darling_executable(lastcomm lastcomm/lastcomm.c)
add_darling_executable(logname logname/logname.c)
add_darling_executable(path_helper path_helper/path_helper.c)
add_darling_executable(expr ${CMAKE_CURRENT_BINARY_DIR}/expr.c)

install(TARGETS date echo pwd kill sleep expr
	DESTINATION libexec/darling/bin)

install(TARGETS env which basename dirname apply nohup getopt true false killall mktemp
	nice printenv printf renice script seq systime shlock id
	tee time uname jot lastcomm logname
	users what whereis xargs find hexdump hostname yes
	DESTINATION libexec/darling/usr/bin)

install(TARGETS path_helper DESTINATION libexec/darling/usr/libexec
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

InstallSymlink(id libexec/darling/usr/bin/groups)
InstallSymlink(id libexec/darling/usr/bin/whoami)

set(man1
	alias/alias.1
	alias/builtin.1
	apply/apply.1
	basename/basename.1
	basename/dirname.1
	date/date.1
	echo/echo.1
	env/env.1
	expr/expr.1
	false/false.1
	find/find.1
	getopt/getopt.1
	hexdump/hexdump.1
	hexdump/od.1
	hostname/hostname.1
	id/groups.1
	id/id.1
	id/whoami.1
	jot/jot.1
	kill/kill.1
	killall/killall.1
	lastcomm/lastcomm.1
	locate/locate/locate.1
	logname/logname.1
	mktemp/mktemp.1
	nice/nice.1
	nohup/nohup.1
	printenv/printenv.1
	printf/printf.1
	pwd/pwd.1
	script/script.1
	seq/seq.1
	shlock/shlock.1
	sleep/sleep.1
	su/su.1
	systime/systime.1
	tee/tee.1
	test/test.1
	time/time.1
	true/true.1
	uname/uname.1
	users/users.1
	w/uptime.1
	w/w.1
	what/what.1
	whereis/whereis.1
	which/which.1
	who/who.1
	xargs/xargs.1
	yes/yes.1
)

install(FILES ${man1} "test/[.1" DESTINATION libexec/darling/usr/share/man/man1)

set(man8
	chroot/chroot.8
	locate/bigram/locate.bigram.8
	locate/code/locate.code.8
	locate/locate/locate.updatedb.8
	path_helper/path_helper.8
	renice/renice.8
)

install(FILES ${man8} DESTINATION libexec/darling/usr/share/man/man8)
