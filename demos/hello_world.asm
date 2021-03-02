SET message "\e[33;1mHello World!\e[0m\n"
SET message_end 0

TAG MAIN

MOV EAX 0
LEA RBX [message]
LEA RCX [message_end - message]

CALL WRITE

END