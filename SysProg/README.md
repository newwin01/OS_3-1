## Sysprog_assignment1
### PCTest: Multi-processed Programming Assignment Testers

### *Contributer: Sechang Jang*

---
PCtest displays the summary of the test results (see Section 2.3) via the standard output. The summary must include (1) whether the compilation was succeeded or not, (2) the number of the correct test executions and the number of failed test executions, (3) the maximum and the minimum time of a single test execution, and the sum of all test execution time of target.

Given source code files, pctest uses a compiler (e.g., gcc) to 
build two executable files respectively, and then runs these two 
executables  with  each  of  the  test  input  file  in  Tests.  pctest 
determines whether the execution result of target for an input is 
correct or not by checking if the program terminated successfully 
and the texts produced to the standard output is identical to one 
that produced by solution. 

It works with following options

> *i: directory that contains testcases | t: file to store the result (optional) <br>*

> After that, provide solution source code and target source code. Program will automatically compile the source as binary file and compare the result. 

> *make* <br>
./pctest -i *\<testdir>* -t *\<timeout>* *\<solution>* *\<target>* <br>


<br>
<br>