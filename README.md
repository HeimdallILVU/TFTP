# TFTP Code for the ENSEA Base Reseau LAB

## How to use 
    ./tftp gettftp [host] [file] to get a file from the host
    ./tftp puttftp [host] [file] to send a file to the host

## Question 1
    At this stage, the code simply parse the arguments.
        -  args[1] is the mode either `puttftp` or `gettftp`
        -  args[2] is the host ip or url
        -  args[3] is the file name

## Question 2
    As done in the TDm3, we use the getaddrinfo function to get the information about the server, mainly the ip adress.

## Question 3
    Using the information previously obtained, we create a socket.

## Question 4
    The structure of the RRQ is : 0x00 0x01 [filename] 0x00 [mode] 0x00
    To ACK an incoming packet, we take the incoming packet and return it with the first two bit set to 0x00 0x04
    and we remove all the bytes after the fourth (4) one.

## Question 5
    The structure of the WRQ is : 0x00 0x02 [filename] 0x00 [mode] 0x00

    ACK is this time received. We need to keep track of the number of block sent.

## Conclusion
    We encountered a major issue. We could send packet to the server, which would send a packet back. But somewhere between the computer and the program, the incoming packet gets deflected, and a ICMP packet with the incoming data gets sent.

    Despite this problem, the logic for is in the code and eveything should be working fine if this issue is resovled.

    Merry Xmas.


