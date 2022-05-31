### HOW TO USE 
you can compile every .o file and launch file using 'make all' <br>
According to assigment requirement in the makefile there is a command 'make Ex6' which link ALL the .o files into a shared object. This method won't work because there are about 6 main function in this library. <br>

#### RUN QUESTIONS 1-3:
to run server AO: <br>
run 'make main1' and then run the server using ./main1 <br>
run 'make AOclient' and then run the client using ./client localhost (make sure the server is running)  <br>
run 'make testAO' and then run the tests using ./testAO localhost (make sure the server is running, and the server is frest to pass the tests) <br>

#### RUN QUESTION 4:
run 'make guardtest' and then run the test using ./guardtest<br>

#### RUN QUESTION 5:
run 'make singleton' and then run the test using ./singleton<br>

#### RUN QUESTION 6:
to run the server: 'make pollserver' and then './pollserver'
to run the client: 'make pollclient' and then './pollclient'