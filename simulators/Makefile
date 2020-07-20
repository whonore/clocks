all: binclock

binclock: binclock.c arduino.h
	gcc -Wall binclock.c -o binclock -lm

binclock.c: ../binclock.ino
	python ino2c.py ../binclock.ino binclock.c

lintpy: $(wildcard *.py)
	python -m black .
	python -m isort --profile=black .
	python -m mypy --strict .
	python -m flake8 --ignore=E501,W503 .
