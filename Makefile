#Makefile (proj2)

# s is for silence you imbecile
MAKEFLAGS += -s
# Build Castor the Compiler; if (DEBUGMODE) build all tests too
.PHONY: all
all: 
	$(MAKE) -C src/
	#Debug Mode
	#make -f src/Testfile 
# Build new tests
tests:
	make -f src/Testfile
	
# Build all tests
testall:
	make -f src/Testfile
	make -f src/TestfileINTS
	make -f src/TestfileFLOATS
	make -f src/TestfileCONDS

# Run all .out executables
.PHONY: run
run: all testall
	$(foreach dotoutfile,$(wildcard *.out),$(shell $(dotoutfile) >> results.txt 2>> errorOutput))
	
# Remove compilation artifacts
# When possible, remove in reverse order!!!
.PHONY: clean
clean:
	make -f src/Testfile clean
	$(MAKE) -C src/ clean
	-$(RM) Castor *.out 
	
cleanall:
	make -f src/TestfileINTS clean
	make clean
	-$(RM) results.txt errorOutput
	
	
