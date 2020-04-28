all: README

clean:
	rm -f README README.bak

README: servo_tester.ino
	pod2readme $< $@ && rm -f $@.bak
