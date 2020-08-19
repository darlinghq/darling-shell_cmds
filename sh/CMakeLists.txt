project(shell_cmds-sh)

add_compile_definitions(
	SHELL
)

include_directories(
	../gen/sh
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_darling_executable(ash
	alias.c
	arith_yacc.c
	arith_yylex.c
	cd.c
	error.c
	eval.c
	exec.c
	expand.c
	histedit.c
	input.c
	jobs.c
	mail.c
	main.c
	memalloc.c
	miscbltin.c
	mystring.c
	options.c
	output.c
	parser.c
	redir.c
	show.c
	trap.c
	var.c

	bltin/echo.c

	../gen/sh/builtins.c
	../gen/sh/nodes.c
	../gen/sh/syntax.c

	../kill/kill.c
	../printf/printf.c
	../test/test.c
)

target_link_libraries(ash
	edit
)

install(TARGETS ash DESTINATION libexec/darling/bin)

install(FILES sh.1 DESTINATION libexec/darling/usr/share/man/man1 RENAME ash.1)
