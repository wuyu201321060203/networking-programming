##This is a implemention of Sudoku solving server
###It uses the multithreaded programming paradigms and it is also my initial
###step for implementing a framework for handling computing task and user
###callbacks based on threadpool.

##You can use telnet as the client
###eg: telnet localhost 9981
###your input is:
###a:000000010400000000020000000000050407008000300001090000300400200050100000000806000
###Sudoku server will solve the problem and reply you with the answer:
###a:693784512487512936125963874932651487568247391741398625319475268856129743274836159