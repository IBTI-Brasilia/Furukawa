cd  "%~dp0"

:loop

py test_mode.py

ECHO teste realizado com sucesso
set /p delBuild=De novo ???? [s/n]?: 
if %delbuild% == s (
	goto loop
) else ( 
	if %delbuild% == S (
	 goto loop
	) else ( 
	 pause )
	)
)	